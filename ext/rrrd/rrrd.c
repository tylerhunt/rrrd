#include "rrrd.h"


/* extension initialization */

void Init_rrrd() {
  RRRD = rb_define_module("RRRD");
  Error = rb_define_class_under(RRRD, "Error", rb_eStandardError);

  RRD = rb_define_class_under(RRRD, "RRD", rb_cObject);
  rb_define_method(RRD, "initialize", rrrd_rrd_initialize, 1);
  rb_define_method(RRD, "fetch", rrrd_rrd_fetch, -1);
  rb_define_method(RRD, "first", rrrd_rrd_first, -1);
  rb_define_method(RRD, "last", rrrd_rrd_last, 0);
  rb_define_method(RRD, "last_update", rrrd_rrd_last_update, 0);

  Tuple = rb_define_class_under(RRRD, "Tuple", rb_cObject);
  rb_define_method(Tuple, "initialize", rrrd_tuple_initialize, -1);
  rb_funcall(Tuple, rb_intern("attr_reader"), 2, ID2SYM(rb_intern("timestamp")), ID2SYM(rb_intern("values")));
}


/* RRRD::RRD methods */

static VALUE rrrd_rrd_initialize(VALUE self, VALUE filename) {
  rb_iv_set(self, "@filename", filename);
}

static VALUE rrrd_rrd_fetch(int argc, VALUE *argv, VALUE self) {
  VALUE function, options;
  VALUE resolution, starts_at, ends_at;

  rb_scan_args(argc, argv, "11", &function, &options);

  list arguments = rrrd_list_new();
  rrrd_add_filename_arg(self, &arguments);

  // consolidation function
  function = rb_str_new2(rb_id2name(rb_to_id(function)));
  function = rb_funcall(function, rb_intern("upcase"), 0);
  rrrd_add_arg(&arguments, STR2CSTR(function));

  if (RTEST(options)) {
    // extract options from hash
    resolution = rb_hash_aref(options, ID2SYM(rb_intern("resolution")));
    starts_at = rb_hash_aref(options, ID2SYM(rb_intern("starts_at")));
    ends_at = rb_hash_aref(options, ID2SYM(rb_intern("ends_at")));

    // resolution
    if (RTEST(resolution)) {
      resolution = rb_fix2str(resolution, 10);

      rrrd_add_arg(&arguments, "--resolution");
      rrrd_add_arg(&arguments, STR2CSTR(resolution));
    }

    // start time
    if (RTEST(starts_at)) {
      starts_at = rb_funcall(starts_at, rb_intern("to_i"), 0);
      starts_at = rb_fix2str(starts_at, 10);

      rrrd_add_arg(&arguments, "--start");
      rrrd_add_arg(&arguments, STR2CSTR(starts_at));
    }

    // end time
    if (RTEST(ends_at)) {
      ends_at = rb_funcall(ends_at, rb_intern("to_i"), 0);
      ends_at = rb_fix2str(ends_at, 10);

      rrrd_add_arg(&arguments, "--end");
      rrrd_add_arg(&arguments, STR2CSTR(ends_at));
    }
  }

  time_t start, end;
  unsigned long step, ds_count;
  char **raw_names;
  rrd_value_t *raw_data;

  rrd_fetch(arguments.length, arguments.strings, &start, &end, &step, &ds_count, &raw_names, &raw_data);
  rrrd_free_arguments(arguments);
  rrrd_check_for_errors();

  int i;
  int index = 0;
  VALUE data = rb_ary_new();

  for (i = start + step; i <= end; i+= step) {
    int j;
    VALUE timestamp = rb_time_new(i, 0);
    VALUE values = rb_hash_new();

    for (j = 0; j < ds_count; j++) {
      VALUE key = ID2SYM(rb_intern(raw_names[j]));
      rb_hash_aset(values, key, rb_float_new(raw_data[index++]));
    }

    VALUE args[] = { timestamp, values };
    VALUE tuple = rb_class_new_instance(2, args, Tuple);

    rb_ary_push(data, tuple);
  }

  free(raw_data);

  return data;
}

static VALUE rrrd_rrd_first(int argc, VALUE *argv, VALUE self) {
  VALUE index;

  rb_scan_args(argc, argv, "01", &index);

  list arguments = rrrd_list_new();
  rrrd_add_filename_arg(self, &arguments);

  // RRA index
  if (RTEST(index)) {
    index = rb_fix2str(index, 10);
    rrrd_add_arg(&arguments, "--rraindex");
    rrrd_add_arg(&arguments, STR2CSTR(index));
  }

  time_t first = rrd_first(arguments.length, arguments.strings);
  rrrd_free_arguments(arguments);
  rrrd_check_for_errors();

  return rb_time_new(first, 0);
}

static VALUE rrrd_rrd_last(VALUE self) {
  list arguments = rrrd_list_new();
  rrrd_add_filename_arg(self, &arguments);

  time_t last = rrd_last(arguments.length, arguments.strings);
  rrrd_free_arguments(arguments);
  rrrd_check_for_errors();

  return rb_time_new(last, 0);
}

static VALUE rrrd_rrd_last_update(VALUE self) {
  list arguments = rrrd_list_new();
  rrrd_add_filename_arg(self, &arguments);

  time_t last_update;
  unsigned long ds_count;
  char **raw_names, **last_ds;

  rrd_lastupdate(arguments.length, arguments.strings, &last_update, &ds_count, &raw_names, &last_ds);
  rrrd_free_arguments(arguments);
  rrrd_check_for_errors();

  VALUE timestamp = rb_time_new(last_update, 0);
  VALUE values = rb_hash_new();

  int i;

  for (i = 0; i < ds_count; i++) {
    VALUE key = ID2SYM(rb_intern(raw_names[i]));
    VALUE value = rb_funcall(rb_str_new2(last_ds[i]), rb_intern("to_f"), 0);
    rb_hash_aset(values, key, value);
  }

  VALUE args[] = { timestamp, values };
  VALUE tuple = rb_class_new_instance(2, args, Tuple);

  return tuple;
}


/* RRRD::Tuple methods */

static VALUE rrrd_tuple_initialize(int argc, VALUE *argv, VALUE self) {
  VALUE timestamp, values;

  rb_scan_args(argc, argv, "20", &timestamp, &values);

  rb_iv_set(self, "@timestamp", timestamp);
  rb_iv_set(self, "@values", values);
}


/* utility functions */

static list rrrd_list_new() {
  list arguments;

  arguments.length = 1;
  arguments.strings = malloc(sizeof(char *));
  arguments.strings[0] = "";

  return arguments;
};

static void rrrd_free_arguments(list arguments) {
  int i;

  for (i = 0; i < arguments.length; i++) {
    if (i > 0) free(arguments.strings[i]);
  }

  free(arguments.strings);
}

static list rrrd_add_arg(list *arguments, char *value) {
  int i;
  list new_arguments;

  new_arguments.length = (*arguments).length + 1;
  new_arguments.strings = malloc(new_arguments.length * sizeof(char *));

  for (i = 0; i < (*arguments).length; i++) {
    new_arguments.strings[i] = strdup((*arguments).strings[i]);
  }

  rrrd_free_arguments(*arguments);

  new_arguments.strings[new_arguments.length - 1] = strdup(value);
  *arguments = new_arguments;
}

static list rrrd_add_filename_arg(VALUE self, list *arguments) {
  rrrd_add_arg(arguments, STR2CSTR(rb_iv_get(self, "@filename")));
}

static void rrrd_check_for_errors() {
  if (rrd_test_error()) {
    rb_raise(Error, rrd_get_error());
    rrd_clear_error();
  }
}

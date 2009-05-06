#include <ruby.h>
#include <rrd.h>

typedef struct {
  int length;
  char **strings;
} list;

VALUE RRRD, RRD, Tuple, Error;

void Init_rrrd();

// RRRD::RRD methods
static VALUE rrrd_rrd_initialize(VALUE self, VALUE filename);
static VALUE rrrd_rrd_fetch(int argc, VALUE *argv, VALUE self);
static VALUE rrrd_rrd_first(int argc, VALUE *argv, VALUE self);
static VALUE rrrd_rrd_last(VALUE self);
static VALUE rrrd_rrd_last_update(VALUE self);

// RRRD::Tuple methods
static VALUE rrrd_tuple_initialize(int argc, VALUE *argv, VALUE self);

// utility functions
static void rrrd_check_for_errors();
static list rrrd_list_new();
static void rrrd_free_arguments(list arguments);
static list rrrd_add_arg(list *arguments, char *value);
static list rrrd_add_filename_arg(VALUE self, list *arguments);

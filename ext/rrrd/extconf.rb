require 'mkmf'

dir_config('rrrd')
have_library('rrd', 'rrd_first')
create_makefile('rrrd')

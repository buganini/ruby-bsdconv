require 'mkmf'

extension_name = 'bsdconv'

dir_config(extension_name)

have_library('bsdconv', 'bsdconv_create')
have_header('bsdconv.h')

create_makefile(extension_name)


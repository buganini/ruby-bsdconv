Ruby binding API
================

.. rb:module:: bsdconv

.. rb:class:: Bsdconv

	.. rb:const:: FILTER
	.. rb:const:: FROM
	.. rb:const:: INTER
	.. rb:const:: TO

		Phase type

	.. rb:const:: CTL_ATTACH_SCORE
	.. rb:const:: CTL_ATTACH_OUTPUT_FILE
	.. rb:const:: CTL_AMBIGUOUS_PAD

		Request for :rb:meth:`ctl`

	.. rb:classmethod:: new(conversion)

		Create converter instance with given conversion string

	.. rb:classmethod:: insert_phase(conversion, codec, phase_type, phasen)

		Insert conversion phase into bsdconv conversion string

	.. rb:classmethod:: insert_codec(conversion, codec, phasen, codecn)

		Insert conversion codec into bsdconv conversion string

	.. rb:classmethod:: replace_phase(conversion, codec, phase_type, phasen)

		Replace conversion phase in the bsdconv conversion string

	.. rb:classmethod:: replace_codec(conversion, codec, phasen, codecn)

		Replace conversion codec in the bsdconv conversion string

	.. rb:classmethod:: error()

		Get error message

	.. rb:classmethod:: modules_list(type)

		Get modules list of specified type

	.. rb:classmethod:: module_check(type, module)

		Check availability with given type and module name

	.. rb:classmethod:: mktemp(template)

		mkstemp()

	.. rb:classmethod:: fopen(path, mode)

		fopen()

	.. rb:method:: conv(s)

		Perform conversion

	.. rb:method:: init()

		Initialize/Reset bsdconv converter

	.. rb:method:: ctl(arg_ptr_obj, arg_int)

		Manipulate the underlying codec parameters

	.. rb:method:: conv_chunk(s)

		Perform conversion without initializing and flushing

	.. rb:method:: conv_chunk_last(s)

		Perform conversion without initializing

	.. rb:method:: conv_file(from_file, to_file)

		Perform conversion with given filename

	.. rb:method:: counter([name])

		Return conversion info

	.. rb:method:: counter_reset([name])

		Reset counter, if no name supplied, all counters will be reset

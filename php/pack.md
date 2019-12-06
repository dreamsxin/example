# pack

大小端判断

```c
PHP_MINIT_FUNCTION(pack)
{
	int machine_endian_check = 1;
	int i;

	machine_little_endian = ((char *)&machine_endian_check)[0];

	if (machine_little_endian) {
		/* Where to get lo to hi bytes from */
		byte_map[0] = 0;

		for (i = 0; i < (int)sizeof(int); i++) {
			int_map[i] = i;
		}

		machine_endian_short_map[0] = 0;
		machine_endian_short_map[1] = 1;
		big_endian_short_map[0] = 1;
		big_endian_short_map[1] = 0;
		little_endian_short_map[0] = 0;
		little_endian_short_map[1] = 1;

		machine_endian_long_map[0] = 0;
		machine_endian_long_map[1] = 1;
		machine_endian_long_map[2] = 2;
		machine_endian_long_map[3] = 3;
		big_endian_long_map[0] = 3;
		big_endian_long_map[1] = 2;
		big_endian_long_map[2] = 1;
		big_endian_long_map[3] = 0;
		little_endian_long_map[0] = 0;
		little_endian_long_map[1] = 1;
		little_endian_long_map[2] = 2;
		little_endian_long_map[3] = 3;

#if SIZEOF_ZEND_LONG > 4
		machine_endian_longlong_map[0] = 0;
		machine_endian_longlong_map[1] = 1;
		machine_endian_longlong_map[2] = 2;
		machine_endian_longlong_map[3] = 3;
		machine_endian_longlong_map[4] = 4;
		machine_endian_longlong_map[5] = 5;
		machine_endian_longlong_map[6] = 6;
		machine_endian_longlong_map[7] = 7;
		big_endian_longlong_map[0] = 7;
		big_endian_longlong_map[1] = 6;
		big_endian_longlong_map[2] = 5;
		big_endian_longlong_map[3] = 4;
		big_endian_longlong_map[4] = 3;
		big_endian_longlong_map[5] = 2;
		big_endian_longlong_map[6] = 1;
		big_endian_longlong_map[7] = 0;
		little_endian_longlong_map[0] = 0;
		little_endian_longlong_map[1] = 1;
		little_endian_longlong_map[2] = 2;
		little_endian_longlong_map[3] = 3;
		little_endian_longlong_map[4] = 4;
		little_endian_longlong_map[5] = 5;
		little_endian_longlong_map[6] = 6;
		little_endian_longlong_map[7] = 7;
#endif
	}
	else {
		zval val;
		int size = sizeof(Z_LVAL(val));
		Z_LVAL(val)=0; /*silence a warning*/

		/* Where to get hi to lo bytes from */
		byte_map[0] = size - 1;

		for (i = 0; i < (int)sizeof(int); i++) {
			int_map[i] = size - (sizeof(int) - i);
		}

		machine_endian_short_map[0] = size - 2;
		machine_endian_short_map[1] = size - 1;
		big_endian_short_map[0] = size - 2;
		big_endian_short_map[1] = size - 1;
		little_endian_short_map[0] = size - 1;
		little_endian_short_map[1] = size - 2;

		machine_endian_long_map[0] = size - 4;
		machine_endian_long_map[1] = size - 3;
		machine_endian_long_map[2] = size - 2;
		machine_endian_long_map[3] = size - 1;
		big_endian_long_map[0] = size - 4;
		big_endian_long_map[1] = size - 3;
		big_endian_long_map[2] = size - 2;
		big_endian_long_map[3] = size - 1;
		little_endian_long_map[0] = size - 1;
		little_endian_long_map[1] = size - 2;
		little_endian_long_map[2] = size - 3;
		little_endian_long_map[3] = size - 4;

#if SIZEOF_ZEND_LONG > 4
		machine_endian_longlong_map[0] = size - 8;
		machine_endian_longlong_map[1] = size - 7;
		machine_endian_longlong_map[2] = size - 6;
		machine_endian_longlong_map[3] = size - 5;
		machine_endian_longlong_map[4] = size - 4;
		machine_endian_longlong_map[5] = size - 3;
		machine_endian_longlong_map[6] = size - 2;
		machine_endian_longlong_map[7] = size - 1;
		big_endian_longlong_map[0] = size - 8;
		big_endian_longlong_map[1] = size - 7;
		big_endian_longlong_map[2] = size - 6;
		big_endian_longlong_map[3] = size - 5;
		big_endian_longlong_map[4] = size - 4;
		big_endian_longlong_map[5] = size - 3;
		big_endian_longlong_map[6] = size - 2;
		big_endian_longlong_map[7] = size - 1;
		little_endian_longlong_map[0] = size - 1;
		little_endian_longlong_map[1] = size - 2;
		little_endian_longlong_map[2] = size - 3;
		little_endian_longlong_map[3] = size - 4;
		little_endian_longlong_map[4] = size - 5;
		little_endian_longlong_map[5] = size - 6;
		little_endian_longlong_map[6] = size - 7;
		little_endian_longlong_map[7] = size - 8;
#endif
	}

	return SUCCESS;
}
```
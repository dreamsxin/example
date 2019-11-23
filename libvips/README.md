# libvips

https://github.com/libvips/libvips
https://github.com/libvips/php-vips-ext

```shlel
sudo apt-get install libpango1.0-dev
sudo apt-get install gtk-doc-tools
sudo apt-get install gobject-introspection
```

## 编译

```shell
./autogen.sh
./configure --enable-debug=yes --with-pangoft2
make -j4 && sudo make install
```

如果出现错误 `libvips/Makefile.am:77: error: HAVE_INTROSPECTION does not appear in AM_CONDITIONAL` 请安装 `gobject-introspection`

如果出现错误 `config.status: error: cannot find input file: `Makefile.in'` 执行下列命令生成：
```shell
aclocal
automake --add-missing
```

如果出现错误 `configure.ac:353: error: required file './ltmain.sh' not found` 执行下列命令生成：
```shell
libtoolize --version
libtoolize --copy --force --automake
```

```shell
php -d extension=vips.so vips.php
```
```php
#!/usr/bin/env php
<?php

$x = vips_image_new_from_file($argv[1])["out"];
$x = vips_call("invert", $x)["out"];
vips_image_write_to_file($x, $argv[2]);
EOF;
```

## 阅读源码

用到 Glib 库，在 GObject 对象初始化时，会注册类名 `nickname` 也就是 `operation_name`

* `LIBVIPS\libvips\iofuncs\operation.c`
- int vips_call( const char *operation_name, ... )
- VipsOperation *vips_operation_new( const char *name )
通过 `operation_name` 构建 `VipsOperation` 对象
- static int vips_call_by_name( const char *operation_name, const char *option_string, va_list required, va_list optional )
- int vips_call_required_optional( VipsOperation **operation, va_list required, va_list optional )
调用 `vips_cache_operation_buildp`
- int vips_call_argv( VipsOperation *operation, int argc, char **argv )

* `LIBVIPS\libvips\iofuncs\cache.c`
- int vips_cache_operation_buildp( VipsOperation **operation )
调用 `vips_cache_operation_lookup` 或 `vips_object_build`
- VipsOperation *vips_cache_operation_lookup( VipsOperation *operation )
Look up an unbuilt @operation in the cache. If we get a hit, ref and return the old operation. If there's no hit, return NULL.

* `LIBVIPS\libvips\iofuncs\object.c`
- GType vips_type_find( const char *basename, const char *nickname )
`basename` 是基类名，`nickname` 要查找类名，找到之后会调用 `g_object_new` 构建对象
- int vips_object_build( VipsObject *object )
执行操作

## Extending VIPS

* 编写插件 `negative`

```c
typedef struct _Negative {
  VipsOperation parent_instance;

  VipsImage *in;
  VipsImage *out;

  int image_max;

} Negative;

typedef struct _NegativeClass {
  VipsOperationClass parent_class;

  /* No new class members needed for this op.
   */

} NegativeClass;

G_DEFINE_TYPE( Negative, negative, VIPS_TYPE_OPERATION );

static void negative_init( Negative *negative )
{
  negative->image_max = 255;
}

static void negative_class_init( NegativeClass *class )
{
  GObjectClass *gobject_class = G_OBJECT_CLASS( class );
  VipsObjectClass *object_class = VIPS_OBJECT_CLASS( class );

  gobject_class->set_property = vips_object_set_property;
  gobject_class->get_property = vips_object_get_property;

  object_class->nickname = "negative";
  object_class->description = "photographic negative";
  object_class->build = negative_build;

  VIPS_ARG_IMAGE( class, "in", 1, 
    "Input", 
    "Input image",
    VIPS_ARGUMENT_REQUIRED_INPUT,
    G_STRUCT_OFFSET( Negative, in ) );

  VIPS_ARG_IMAGE( class, "out", 2, 
    "Output", 
    "Output image",
    VIPS_ARGUMENT_REQUIRED_OUTPUT, 
    G_STRUCT_OFFSET( Negative, out ) );

  VIPS_ARG_INT( class, "image_max", 4, 
    "Image maximum", 
    "Maximum value in image: pivot about this",
    VIPS_ARGUMENT_OPTIONAL_INPUT,
    G_STRUCT_OFFSET( Negative, image_max ),
    0, 255, 255 );
}

static int negative_build( VipsObject *object )
{
  VipsObjectClass *class = VIPS_OBJECT_GET_CLASS( object );
  Negative *negative = (Negative *) object;

  if( VIPS_OBJECT_CLASS( negative_parent_class )->build( object ) )
    return( -1 );

  if( vips_check_uncoded( class->nickname, negative->in ) ||
    vips_check_format( class->nickname, negative->in, VIPS_FORMAT_UCHAR ) )
    return( -1 );

  g_object_set( object, "out", vips_image_new(), NULL ); 

  if( vips_image_pipelinev( negative->out, 
    VIPS_DEMAND_STYLE_THINSTRIP, negative->in, NULL ) )
    return( -1 );

  if( vips_image_generate( negative->out, 
    vips_start_one, 
    negative_generate, 
    vips_stop_one, 
    negative->in, negative ) )
    return( -1 );

  return( 0 );
}

static int negative_generate( VipsRegion *or, void *vseq, void *a, void *b, gboolean *stop )
{
  /* The area of the output region we have been asked to make.
   */
  VipsRect *r = &or->valid;

  /* The sequence value ... the thing returned by vips_start_one().
   */
  VipsRegion *ir = (VipsRegion *) vseq;

  VipsImage *in = (VipsImage *) a;
  Negative *negative = (Negative *) b;
  int line_size = r->width * negative->in->Bands; 

  int x, y;

  /* Request matching part of input region.
   */
  if( vips_region_prepare( ir, r ) )
    return( -1 );

  for( y = 0; y < r->height; y++ ) {
    unsigned char *p = (unsigned char *)
      VIPS_REGION_ADDR( ir, r->left, r->top + y ); 
    unsigned char *q = (unsigned char *)
      VIPS_REGION_ADDR( or, r->left, r->top + y ); 

    for( x = 0; x < line_size; x++ ) 
      q[x] = negative->image_max - p[x];
  }

  return( 0 );
}
```

Makefile:
```Makefile
# Makefile for vips plugin 

# Object files
OBJS = negative.o 

# Source files
SRC = $(OBJS:.o=.c) 

# Headers
HEADERS = 

# All
TAGS = $(SRC) $(HEADERS)

#linux
CFLAGS += -shared -fPIC 
#solaris
#CXXFLAGS += -Xa -Kpic 
CPPFLAGS += `pkg-config vips --cflags`
LDFLAGS += `pkg-config vips --libs`
OUT = vips-negative.plg

release: $(OUT)
debug: $(OUT)

.PHONY: debug  
debug: CFLAGS += -g

.PHONY: release  
release: CFLAGS += -O3

$(OUT): $(OBJS)
	$(CC) -o $(OUT) -shared $(OBJS)
#solaris
#	ld -o $(OUT) -G $(OBJS)
#	vips -plugin ./$(OUT) resample in.v in2.v out.v 0.1 20 1 1 0
#	vips -plugin ./$(OUT) scale_par matrix.mat out.mat 2.0

clean: 
	$(RM) $(OBJS) 

tags: $(TAGS)
	ctags $(TAGS)

# version as MAJOR.MINOR
VIPS_VERSION = $(shell pkg-config vips --modversion | \
	         awk '{split($$1,a,"."); print a[1]"."a[2]}')
PLUGIN_DIR = $(VIPSHOME)/lib/vips-plugins-$(VIPS_VERSION)

install: $(OUT)
	-mkdir -p $(PLUGIN_DIR)
	-cp $(OUT) $(PLUGIN_DIR)
```
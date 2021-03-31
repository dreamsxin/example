```proto
syntax = "proto3";//文件第一行指定使用的protobuf版本，如果不指定，默认使用proto2。如果指定，则必须在文件的非空非注释的第一行

package protobuf;//定义包，该行是可选的,可以为.proto文件新增一个可选的package声明符，用来防止不同的消息类型有命名冲突

import public "other_protos.proto";//引入其他protobuf文件
import  "google/protobuf/any.proto";

option java_package = "com.protobuf";//声明编译成java代码后的package名称，但是不建议使用，该行是可选的
option java_outer_classname = "PersonJ";//申明转成java代码后的java文件名，可以不指定，默认生成的class是：PersonOuterClass.java;注意不能与下面的message重名了
option optimize_for = SPEED;//可以被设置为 SPEED, CODE_SIZE,or LITE_RUNTIME。这些值将通过如下的方式影响C++及java代码的生成,默认是SPEED，一般不需要设置

//messaage可以理解为java中的class关键字
 message Person {
     //关于变量的定义，格式为：[修饰符][数据类型][变量名] = [唯一编号];//唯一编号是用来标识字段的
     string var1 = 1;
     //string var2 = 1;//该变量定义会编译报错，因为编号1已经被使用了

     /*
        protobuf中的基本数据类型----对应的java基础数据类型
        int32     ----    int
        int64     ----    long
        double    ----    double
        float     ----    float
        bytes     ----    ByteString
        bool      ----    boolean
        此外，还有：
        uint32    ----    int
        uint64    ----    long
        sint32    ----    int
        sint64    ----    long
        fixed32   ----    int
        fixed64   ----    long
        sfixed32  ----    int
        sfixed64  ----    long
     */

     /*
        proto3取消了required和optional两个关键字
        repeated用来定义数组
     */
     repeated string list = 2;
     /*
       使用map
     */
     map<string, string> projects = 23;
     /*
        有时候你需要保留一些你以后要用到的编号或者变量名，使用reserved关键字
     */
     reserved 3, 15, 9 to 11;
     reserved "foo", "bar";

     //string var2 = 3;//编译会报错，因为3被保留了
     //string var3 = 10;//编译会报错，因为10被保留了
     //string foo = 12;//编译会报错，因为foo被保留了

     /*
     由于一些历史原因，基本数值类型的repeated的字段并没有被尽可能地高效编码。在新的代码中，用户应该使用特殊选项[packed=true]来保证更高效的编码。
     注意[packed=true]只能用在 repeated修饰的数字类型中
     */
     repeated int32 var11 = 28 [packed=true];

     /*
        关于字段的默认值：
        string类型的变量，默认值是空字符串
        bytes类型的变量，默认值是空byte数组
        bool类型的变量，默认值是false
        数字类型的变量，默认值是0
        枚举类型的变量，默认值是第一个枚举值,而且这个第一个枚举值的数字值必须是0

     */

     /*
        定义枚举
        一个enum类型的字段只能用指定的常量集中的一个值作为其值（如果尝 试指定不同的值，解析器就会把它当作一个未知的字段来对待）
     */

     enum Corpus {
         UNIVERSAL = 0;//第一个枚举值，这里的数字必须是0，不然编译不通过
         WEB = 1;
         //WEB1 = 1;//这里编译不通过，数字1只能对应一个枚举值。
         IMAGES = 2;
         LOCAL = 3;
         NEWS = 4;
         PRODUCTS = 5;
         VIDEO = 6;
     }
     Corpus corpus = 4;

     /*
        你可以为枚举常量定义别名。 需要设置allow_alias option 为 true, 否则 protocol编译器会产生错误信息。
     */
     enum EnumAllowingAlias {
         option allow_alias = true;
         UNKNOWN = 0;
         STARTED = 1;
         RUNNING = 1;
     }

    /*
        Message Type 作为变量
    */
     Test test = 14;//同一个包下的other_protos.proto文件中的message Test作为变量的类型

    /*
        嵌套的message，message可以无限嵌套
    */
     message Result {
         string url = 1;
         string title = 2;
         repeated string snippets = 3;
     }
     repeated Result results = 16;
     repeated Person.Result results1 = 17;//也可以这样定义

     /*
        使用Any变量，用于定义任意的值
     */
     repeated google.protobuf.Any details = 21;

     /*
        使用Oneof变量
        如果你的消息中有很多可选字段， 并且同时至多一个字段会被设置， 你可以加强这个行为，使用oneof特性节省内存.
        Oneof字段就像可选字段， 除了它们会共享内存， 至多一个字段会被设置。 设置其中一个字段会清除其它oneof字段。 你可以使用case()或者WhichOneof() 方法检查哪个oneof字段被设置， 看你使用什么语言了.
        你可以增加任意类型的字段, 但是不能使用 required, optional, repeated 关键字.
        在产生的代码中, oneof字段拥有同样的 getters 和setters， 就像正常的可选字段一样. 也有一个特殊的方法来检查到底那个字段被设置. 你可以在相应的语言API中找到oneof API介绍
        设置oneof会自动清楚其它oneof字段的值. 所以设置多次后，只有最后一次设置的字段有值.
     */
     oneof test_oneof {
         string name = 24;
         Result sub_message = 29;
     }
}


/*
    定义服务
*/
//service SearchService {
//    rpc Search (SearchRequest) returns (SearchResponse);
//}
```

# FANN初使用

FANN库的使用是比较简单的，代码也好懂。这个源代码主要分为三个部分：生成训练集，建立并训练神经网络，测试神经网络。

## 生成训练集

将数据输出到 `add.fann`，每组数据都是符合“输入1+输入2=输出1”的，这是我们期待神经网络能“发现”的。
```c
int     Num = 200;
float   Mf = Num*3.f;
int     i;
double  a, b, c;
FILE   *fp;
fopen_s(&fp, "add.fann", "w");
fprintf_s(fp, "%d 2 1\n", Num);
// 生成训练文件
for (i = 1; i <= Num; i++) {
    // 生成2个数, 要求在(0,1)之间
    a = i / Mf;
    b = (i + 1) / Mf;
    c = a + b;    // 要求在(0,1)之间
 
		  // 输入内容写到训练文件
    fprintf_s(fp, "%lf %lf\n%lf\n", a, b, c);
}
fclose(fp);
```

## 训练神经网络

```c
const unsigned int num_input = 2;       // 输入项个数
const unsigned int num_output = 1;      // 输出项个数
const unsigned int num_layers = 3;      //神经网络层数
const unsigned int num_neurons_hidden = 3; //隐藏层有3个神经元
const float desired_error = (const float) 0.00000001;//均方误差
const unsigned int max_epochs = 500000; // 最多执行次数   
const unsigned int epochs_between_reports = 10000;   // 报告频率
 
struct fann *ann; //声明了一个指向神经网络的指针
 
// 样本训练
//是一个可变参数函数，
//第一个参数表明了神经网络层数（包括输入输出层），其余参数从输入层开始逐层表示各层的输入项个数或者神经元数量或者输出项个数，不得不说这是一个精巧方便的设计。
ann = fann_create_standard(num_layers, num_input, num_neurons_hidden, num_output);
 
//设定激活函数
fann_set_activation_function_hidden(ann, FANN_LINEAR);  //设定所有隐藏层的所有神经元的激活函数，如果要为每个神经元单独设置输出函数，见 fann_set_activation_function 。
fann_set_activation_function_output(ann, FANN_LINEAR);  //设定输出层的激活函数

//用文件 add.fann 存储的训练集训练ann指向的神经网络。
//训练分为多个轮次（epochs），每个轮次跑一次完整的数据集
//max_epochs 指出了最大训练轮次
//epochs_between_reports 则规定了每隔多少次在屏幕上打印一次当前进度
//desired_error 是我们要求的神经网络输出与期望输出的差距（其实是均方误差）的上限，当神经网络输出结果与期望结果的差距小于 desired_error 时训练就结束了。
//这个例子中我们规定最多执行50000次，每10000次报告一次，当均方误差低于0.00000001或达到最大训练次数时停止训练。
fann_train_on_file(ann, "add.fann", max_epochs, epochs_between_reports, desired_error); 

fann_save(ann, "add.fann.net");//将训练完成的神经网络保存至文件 add.fann.net 中。
fann_destroy(ann);
```

## 测试神经网络

```c
void exec(double a, double b)
{
    struct fann *ann;
 
    fann_type *calc_out;
    fann_type input[2];
    ann = fann_create_from_file("add.fann.net");
 
    input[0] = a;
    input[1] = b;
 
    calc_out = fann_run(ann, input);
    fann_destroy(ann);
    printf("输入a=%f\nb=%f\n输出c=%f\n期望值c=%f\n\n", input[0], input[1], calc_out[0], input[0] + input[1]);
}
```

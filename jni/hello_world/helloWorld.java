public class helloWorld{
	public static void main(String[] args){
		System.out.println("Hello, World");
	}
	public static int square(int input){
		int output = input * input;
		return output;
	}
	public static int power(int input, int exponent){
		int output,i;
		output=1;
		for(i=0;i<exponent;i++){
			output *= input;
		}
		return output;
	}
}


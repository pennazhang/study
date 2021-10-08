package com.harman.test;

/*
Running Result:
work 2
work 3
work 4
Last result = 4
 */

public class Main {

    public static int divsion(int f1, int f2) {
        return f1 / f2;
    }

    public static int test() {
        try {
            printStep(2);
            int a = divsion(5, 0);
            return 2;
        }
        catch(ArithmeticException e) {
            printStep(3);
            return 3;
        }
        finally {
            printStep(4);
            return 4;
        }
    }

    public static void main(String[] args) {
        int result = test();
        System.out.println("Last result = " + result);
    }

    public static void printStep(int i)
    {
        System.out.println("work " + i);
    }
}



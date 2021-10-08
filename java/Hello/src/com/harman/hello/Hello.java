package com.harman.hello;

import java.util.Scanner;

public class Hello {
    public static void main(String[] args) {
        System.out.print("Please enter your name: ");
        Scanner scanner = new Scanner(System.in);
        String name = scanner.nextLine();

        System.out.print("Please enter your age: ");
        int age = scanner.nextInt();

        System.out.print("Please enter you id: ");
        String id = scanner.next();

        System.out.println("Name: " + name);
        System.out.println("age: " + age);
        System.out.println("id: " + id);
    }
}

package com.harman.serialize;

import java.io.Serializable;

public class Person  implements Serializable {
    private int age;
    private String name;
    private String id;

    public Person(int age, String name, String id) {
        this.age = age;
        this.id = id;
        this.name = name;
    }

    public int getAge() {
        return age;
    }

    public void setAge(int age) {
        this.age = age;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    void print() {
        System.out.println("name: " + name);
        System.out.println("id: " + id);
        System.out.println("age: " + age);
    }
}

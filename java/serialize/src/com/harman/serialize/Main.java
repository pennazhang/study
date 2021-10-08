package com.harman.serialize;

import java.io.*;

public class Main {
    public static void main(String[] args) {
        Person Mike = new Person(16, "Mike Hu", "19490401");
        if (serializePerson(Mike) == false)
        {
            return;
        }

        Person anotherOne = deserializePerson();
        anotherOne.print();
    }

    static boolean serializePerson(Person person)  {
        try {
            ObjectOutputStream oo = new ObjectOutputStream(new FileOutputStream(new File("Person.txt")));
            oo.writeObject(person);
            oo.close();
            return true;
        }
        catch (IOException e)
        {
            System.out.println("Failed to serialize object to Person.txt");
            e.printStackTrace();
            return (false);
        }
    }

    static Person deserializePerson() {
        try {
            ObjectInputStream ois = new ObjectInputStream(new FileInputStream(new File("Person.txt")));
            Person person = (Person)ois.readObject();
            return person;
        }
        catch (IOException | ClassNotFoundException e)
        {
            System.out.println("Failed to deserialize object to Person.txt");
            e.printStackTrace();
            return null;
        }
    }
}

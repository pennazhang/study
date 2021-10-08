package com.harman.txtFile;

import java.io.*;
import java.nio.CharBuffer;
import java.sql.Time;
import java.text.SimpleDateFormat;
import java.time.LocalDateTime;
import java.time.ZoneId;
import java.util.Date;

public class Main {
    public static void main(String[] args) {

        /** Use FileReader & FileWriter  */
        System.out.println("\n Using FileReader - ");
        writeTest_FileWriter("test1.txt");
        readTest_FileReader("test1.txt");
        System.out.println("\n Using FileReader again - ");
        readTest_FileReader2("test1.txt");

        System.out.println("\n Using FileInputStream again - ");
        writeTest_FileOutputStream("test2.txt");
        readTest_FileInputStream("test2.txt");

        System.out.println("\n Using BufferedReader - ");
        /** Use BufferedWriter & BufferedReader */
        writeTest_BufferedWriter("test.txt");
        readTest_BufferedReader("test.txt");
    }

    static void writeTest_FileOutputStream(String fileName) {
        try {
            // 创建字节输出流对象
            // FileOutputStream(File file)
            // File file = new File("fos.txt");
            // FileOutputStream fos = new FileOutputStream(file);
            // FileOutputStream(String name)
            FileOutputStream fos = new FileOutputStream(fileName);
            /*
             * 创建字节输出流对象了做了几件事情：
              A:调用系统功能去创建文件
              B:创建fos对象 C:把fos对象指向这个文件
             */

            // 写数据
            fos.write("hello,IO\n".getBytes());
            fos.write("hello,Java\n".getBytes());

            // 释放资源
            // 关闭此文件输出流并释放与此流有关的所有系统资源。
            fos.close();
            /*
             * 为什么一定要close()呢?
             * A:让流对象变成垃圾，这样就可以被垃圾回收器回收了
             * B:通知系统去释放跟该文件相关的资源
             */
            // java.io.IOException: Stream Closed
            // fos.write("java".getBytes());
        }
        catch ( IOException e) {
            System.out.println("读取过程存在异常");
        }
    }

    static void readTest_FileInputStream(String fileName) {
        try {
            //建立链接
            FileInputStream fileInputStream = new FileInputStream(fileName);
            int n = 0;
            StringBuffer sBuffer = new StringBuffer();
            for(;;)
            {
                n = fileInputStream.read();//读取文件的一个字节(8个二进制位),并将其由二进制转成十进制的整数返回
                if (n == -1)
                {
                    //当n等于-1,则代表未到末尾.
                    break;
                }
                char by = (char) n; //转成字符
                sBuffer.append(by);
            }
            System.out.println(sBuffer.toString());
        } catch (
                FileNotFoundException e) {
            System.out.println("文件不存在或者文件不可读或者文件是目录");
        } catch (IOException e) {
            System.out.println("读取过程存在异常");
        }
    }

    static void writeTest_FileWriter(String fileName) {
        try {
            FileWriter writer = new FileWriter(fileName);
            writer.write("Hello Kuka:\n");
            writer.write("  My name is coolszy!\n");
            writer.write("  I like you and miss you。");
            LocalDateTime localDateTime = LocalDateTime.now();
            writer.write("  Current Time: " + localDateTime + "\n");
            writer.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    static void readTest_FileReader(String fileName) {
        FileReader fr = null;
        try {
            fr = new FileReader(fileName);
            int ch = 0;
            while ((ch = fr.read()) != -1) //记住即可，read方法如果没有可读取的了，则返回-1，所以就是一直读取，并将
            //读取的内容存入ch，一直到结尾
            {
                System.out.print((char) ch);//打印读取的结果，由于ch是int类型，将其强制转换为String类型
            }
        } catch (IOException e) {
            System.out.println(e.toString());
        } finally {
            try {
                if (fr != null) {
                    fr.close();
                }
            } catch (IOException e) {
                System.out.println(e.toString());
            }
        }
    }

    static void readTest_FileReader2(String fileName) {

        FileReader fr = null;
        try {
            fr = new FileReader(fileName);
            char[] buf = new char[16];
            //read(char [])返回读到的字符个数
            int num = 0;
            while ((num = fr.read(buf)) != -1) //读取文件并把它存入buf中，用num返回读到字符的个数，一直读到结尾
            {
                System.out.print(new String(buf, 0, num));//字符数组里仍有空白没有读入的位置，所以不要了
                //new String(字符串，开始位置，结尾位置)
            }
        } catch (IOException e) {
            System.out.println(e.toString());
        } finally {
            try {
                if (fr != null) {
                    fr.close();
                }
            } catch (IOException e) {
                System.out.println(e.toString());
            }
        }
    }

    static void writeTest_BufferedWriter(String fileName) {
        try (BufferedWriter out = new BufferedWriter(new FileWriter(fileName))) {
            out.write("Hello Kuka:");
            out.newLine();  //注意\n不一定在各种计算机上都能产生换行的效果
            out.write("  My name is coolszy!\n");
            out.write("  I like you and miss you.\n");
            LocalDateTime localDateTime = LocalDateTime.now(ZoneId.of("Asia/Hong_Kong"));
            out.write("  Current Time: " + localDateTime + "\n");
            out.close();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    static void readTest_BufferedReader(String fileName) {
        String line = "";
        try {
            BufferedReader in = new BufferedReader(new FileReader(fileName));
            line = in.readLine();
            while (line != null) {
                System.out.println(line);
                line = in.readLine();
            }
            in.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}

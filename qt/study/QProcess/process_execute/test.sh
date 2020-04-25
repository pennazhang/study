
#!/bin/sh
for ((a=1,b=10;a<=10;a++,b--))
do
    sleep 1
    echo "$a - $b"
done

#include <QTextStream>
#include <QThread>
#include <QProcess>
#include <QString>
#include <QSet>
#include <QtCore>

class Node  
{ 
public:
	Node() : cx(0), cy(0) { };
	Node(int x, int y) : cx(x), cy(y) { };
    bool operator == (const Node &b) const  
    {  
        return (cx==b.cx && cy==b.cy);  
    }  

public:
    int cx, cy;  
};  
  
uint qHash(const Node & key)  
{  
    return key.cx + key.cy;  
}  


int main(int argc, char **argv)
{
#ifdef _WINDOWS_
	/* Memory Leak Checking */
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	
    QCoreApplication app(argc, argv);  
  
    QSet<Node> ss;  
    Node temp;  
    int i, j;  
    for(i = 0, j = 100; i < 10; i += 3, j -= 5)  
    {  
        temp.cx = i;  
        temp.cy = j;  
        ss.insert(temp);  
    }  
	ss.insert(Node(9, 85));

	ss << Node(9, 85) << Node(2, 85) << Node(0, 100);

    QSet<Node>::iterator iter;  
    for(iter=ss.begin(); iter!=ss.end(); ++iter)
	{
        qDebug() << iter->cx << "  " << iter->cy;  
	}  

	if (ss.contains(Node(0, 100)))
	{
		qDebug("Find Node(0, 100)");
	}
	else
	{
		qDebug("Can't find Node(0, 100)");
	}

	qDebug() << "Deleting data (0, 100)";
	ss.remove(Node(0, 100));

	if (ss.contains(Node(0, 100)))
	{
		qDebug("Find Node(0, 100)");
	}
	else
	{
		qDebug("Can't find Node(0, 100)");
	}

    QSet<Node>::const_iterator it = ss.constBegin();
    while (it != ss.constEnd()) 
	{
        qDebug() << it->cx << "  " << it->cy;
        ++it;
    }
}

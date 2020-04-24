#include <QTextStream>
#include <QThread>
#include <QProcess>
#include <QString>
#include "mainApplication.h"
#include <QFile>     
#include <QtXml>  
#include <QDomDocument>   

MainApplication *g_pApp = NULL;
QTextStream *g_pCout = NULL;

bool xml_parse(QString file_name)  
{  
    if(file_name.isEmpty())
	{
		*g_pCout << "file_name is NULL" << endl;
		return false;  
	}

    QFile file(file_name);  
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {  
//       QMessageBox::information(NULL, QString("title"), QString("open error!"));
		*g_pCout << "Failed to open file:" << file_name << endl;
		return false;  
    }
  
    QDomDocument document;  
    QString error;  
    int row = 0, column = 0;  
	if(!document.setContent(&file, false, &error, &row, &column))
    {  
//       QMessageBox::information(NULL, QString("title"), QString("parse file failed at line row and column") + QString::number(row, 10) + QString(",") + QString::number(column, 10));
		*g_pCout << "Failed to setContent, error = " << error << ", row = " << row << endl;
		file.close();
		return false;  
    }  
	
	file.close();
	if(document.isNull())
    {  
 //      QMessageBox::information(NULL, QString("title"), QString("document is null!"));
		*g_pCout << "document is NULL" << endl;
		 return false;  
    }  
  
    QDomElement root = document.documentElement();  

   //root_tag_name is persons
    QString root_tag_name = root.tagName();
   if(root.hasAttribute("name"))
    {
		//name should be Qt
		QString name = root.attributeNode("name").value(); 
		*g_pCout << root_tag_name << "=" << name << endl; 
    }
      
    //get the first node.
    QDomElement person = root.firstChildElement();  
   if(person.isNull()) 
       return true; 

    QString person_tag_name = person.tagName();

    //id should be 1
    QString id = person.attributeNode("id").value();

    //get the child node list.
    QDomNodeList list = root.childNodes();
    int count = list.count();
    for(int i=0; i < count; i++)
    {
       QDomNode dom_node = list.item(i);
       QDomElement element = dom_node.toElement();

	   QString tag_name = element.tagName();

       //get the value for "id". id1 should be equal as id2.
       QString id_1 = element.attributeNode("id").value(); 
       QString id_2 = element.attribute("id");
	   *g_pCout << "\t tag_name=" << tag_name << ",   id = " << id_1 << endl; 

       //get the child nodes. cound = 4, including: name, age, email and website
       QDomNodeList child_list = element.childNodes();
       int child_count = child_list.count();
       for(int j=0; j < child_count; j++)
       {
           QDomNode child_dom_node = child_list.item(j);
           QDomElement child_element = child_dom_node.toElement();
           QString child_tag_name = child_element.tagName();
           QString child_tag_value = child_element.text();
		   *g_pCout << "\t\t" << child_tag_name << " = " << child_tag_value << endl;
       }
    }

   // another way to access child node.
    QDomElement element = person.firstChildElement();  
    while (!element.isNull())
    {  
       QString tag_name = element.tagName();
       QString tag_value = element.text();
       element = element.nextSiblingElement();  
    } ;

	return true;
} 

int main(int argc, char **argv)
{
	QString strCmd;
    MainApplication app(argc, argv);
	g_pApp = &app;

	QTextStream cout(stdout);
	QTextStream cin(stdin);
	g_pCout = &cout;

	cout << "Hello, world!\n";
#ifdef _WINDOWS_	
	xml_parse("c:\\temp\\test.xml");
#endif

#ifdef _LINUX_	
	xml_parse("/git/temp/test.xml");
#endif
	
	return g_pApp->exec();
}

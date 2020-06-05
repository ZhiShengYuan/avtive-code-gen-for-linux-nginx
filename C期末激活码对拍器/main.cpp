#include <cstdio>
#include <iostream>
#include <string>
#include <fstream>
using namespace std;
int magicNumbers[] = { 10690, 12251, 17649, 24816, 33360, 35944, 36412, 42041, 42635, 44011, 53799, 56181, 58536, 59222, 61041 };
string GetNowTime()
{
    time_t tt = time(NULL);
    struct tm* stm = localtime(&tt);
    char tmp[32];
    sprintf(tmp, "%04d-%02d-%02d-%2d", 1900 + stm->tm_year, 1 + stm->tm_mon, stm->tm_mday, stm->tm_hour);
    return tmp;
}
int f1(int n, int byte, int c)
{
    for (int bitIndex = 0; bitIndex <= 7; bitIndex++)
    {
        int bit = (byte >> bitIndex) & 1;
        if (bit + ((n - bit) & ~1) == n)
        {
            n = (n - bit) >> 1;
        }
        else
        {
            n = ((c - bit) ^ n) >> 1;
        }
    }
    return n;
}
string genPassword(string str, int h)
{
    int hash = h;
    for (int byteIndex = str.length() - 1; byteIndex >= 0; byteIndex--) //可以放心从size_t转int，不存在丢失数据，数据大小使然
    {
        hash = f1(hash, (int)str[byteIndex], 0x105C3);
    }
    int n1 = 0;
    while (f1(f1(hash, n1 & 0xFF, 0x105C3), n1 >> 8, 0x105C3) != 0xA5B6)
    {
        if (++n1 >= 0xFFFF)
        {
            cerr << ("Failed to find a key!");
            return "";
        }
    }
    n1 = int(((n1 + 0x72FA) & 0xFFFF) * 99999.0 / 0xFFFF);
    string n1str = "0000" + to_string(n1);
    n1str = n1str.substr(n1str.length() - 5, 5);

    int temp = stoi(n1str.substr(0, n1str.length() - 3) + n1str.substr(n1str.length() - 2, 2) + n1str.substr(n1str.length() - 3, 1));
    temp = int((temp / 99999.0) * 0xFFFF) + 1;
    temp = f1(f1(0, temp & 0xFF, 0x1064B), temp >> 8, 0x1064B);

    for (int byteIndex = str.length() - 1; byteIndex >= 0; byteIndex--)
    {
        temp = f1(temp, (int)str[byteIndex], 0x1064B);
    }

    int n2 = 0;
    while (f1(f1(temp, n2 & 0xFF, 0x1064B), n2 >> 8, 0x1064B) != 0xA5B6)
    {
        if (++n2 >= 0xFFFF)
        {
            cerr << ("Failed to find a key!");
            return "";
        }
    }
    n2 = int((n2 & 0xFFFF) * 99999.0 / 0xFFFF);
    string n2str = "0000" + to_string(n2);
    n2str = n2str.substr(n2str.length() - 5, 5);
    char pass[] =
    {
        n2str[3],
        n1str[3],
        n1str[1],
        n1str[0],
        '-',
        n2str[4],
        n1str[2],
        n2str[0],
        '-',
        n2str[2],
        n1str[4],
        n2str[1]
    };
    string password = string(pass).substr(0, 12);
    return password;
}
void MakeSomeNewGen(string time,string uuid)//CreatGen
{
    string systemguid = uuid;
    string days = "0000000000123456";
    for (int i = 0; i < 10; i++)
    {
        days[i] = time[i];//copy string
    }
    string activationKey = days;
    ofstream WriteGen;
    WriteGen.open("index.html");
    WriteGen << "you can only use it after "<<GetNowTime()<<" one hour <br>\n";
    system("mkdir verify");
    for (auto h : magicNumbers)
    {
        string tmp;
        tmp = genPassword(systemguid + "$1&" + activationKey, h);
        WriteGen << tmp <<"<br>"<< endl;
        string cmd = "touch ./verify/" + tmp + ".html";
        char* a = (char*)cmd.c_str();
        system(a);
        ofstream writeverify("./verify/" + tmp + ".html");
        writeverify << systemguid;
        writeverify.close();
    }
    WriteGen.close();
    system("chmod 777 index.html");//防止写入失败
}
int main()
{
    system("rm -rf verify");
#ifdef linux
    system("uuidgen > uuid.txt");//download random uuid.
#endif
#ifdef WIN32
    system("powershell -Command "[guid]::NewGuid().ToString()" > uuid.txt");
#endif // WIN32

    ifstream readuuid("uuid.txt");
    string t; readuuid >> t;
    readuuid.close();
#ifdef linux
    system("rm uuid.txt");
#endif
#ifdef WIN32
    system("del uuid.txt");
#endif
    MakeSomeNewGen(GetNowTime(),t);
    return 0;
}

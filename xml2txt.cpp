#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

class Note
{
    public:
        Note() {}
        Note(char step_, char octave_, string alter_, string type_, bool rest_):
            step(step_), octave(octave_), type(type_),
            alter(alter_), rest(rest_) {}

        string alter;
        string type;
        char step;
        char octave;
        bool rest;

        string encode()
        {
            char t = 0;
            if(type == "whole")  t = 1;
            if(type == "half")  t = 2;
            if(type == "quarter")  t = 4;
            if(type == "eighth")  t = 8;
            if(type == "16th")  t = 16;
            if(type == "32nd")  t = 32;

            string tString = "00";
            if(t < 10)
                tString = "0" + patch::to_string((int)t);
            else
                tString = patch::to_string((int)t);

            string restString = "0";
            if(rest)
            {
                return "1" + tString + "00";
            }


            char note = 0;
            // determine note
            switch( step )
            {
                case 'A': note = 1;
                    break;
                case 'B': note = 3;
                    break;
                case 'C': note = 4;
                    break;
                case 'D': note = 6;
                    break;
                case 'E': note = 8;
                    break;
                case 'F': note = 9;
                    break;
                case 'G': note = 11;
                    break;
            }
            // shift note by octave
            note += 12 * (int)(octave - '0');
            // shift note by alter
            if(alter[0] == '-')
            {
                note -= (int)(alter[1] - '0');
            }
            else
            {
                note += (int)(alter[0] - '0');
            }

            string noteString = "00";
            if(note < 10)
                noteString = "0" + patch::to_string((int)note);
            else
                noteString = patch::to_string((int)note);



            return "" + restString + tString + noteString;
        }
};

string getNextTag(std::ifstream & input)
{
    while(input.get() != '<') {}

    string tagname = "";
    while(input.peek() != '>')
    {
        tagname += input.get();
    }
    input.get();

    return tagname;
}

string getAttrContents(stringstream & input)
{
    string contents = "";
    while(contents.find("</") == -1)
    {
        contents += input.get();
    }
    contents.erase(contents.end()-2, contents.end());

    return contents;
}

string getNextTagSS(stringstream & input)
{
    while(input.get() != '<') {}

    string tagname = "";
    while(input.peek() != '>')
    {
        tagname += input.get();
    }

    input.get();

    return tagname;
}

Note parseNote(string data)
{
    char step = 0;
    char octave = 0;
    string type = "";
    string alter = "0";
    bool rest = false;

#if 0
    cout << data << endl;
#endif

    if(data.find("rest") != -1)
        rest = true;

    stringstream dataStream(data);

    bool tagFound = false;

    if(!rest)
    {
        while(!tagFound)
        {
            if(getNextTagSS(dataStream).substr(0,4) == "step")
                tagFound = true;
        }
        step = getAttrContents(dataStream)[0];
        tagFound = false;

        if(data.find("alter") != -1)
        {
            while(!tagFound)
            {
                if(getNextTagSS(dataStream).substr(0,5) == "alter")
                    tagFound = true;
            }
            alter = getAttrContents(dataStream);
            tagFound = false;
        }

        while(!tagFound)
        {
            if(getNextTagSS(dataStream).substr(0,6) == "octave")
                tagFound = true;
        }
        octave = getAttrContents(dataStream)[0];
        tagFound = false;
    }

    if(data.find("type") != -1)
    {
        while(!tagFound)
        {
            if(getNextTagSS(dataStream).substr(0,4) == "type")
                tagFound = true;
        }
        type = getAttrContents(dataStream);
    }
    else { type = "whole"; }
    Note note(step, octave, alter, type, rest);

    return note;
}

Note getNextNote(std::ifstream & input)
{
    bool noteTagFound = false;
    while(!noteTagFound)
    {
        string temp = getNextTag(input);
        if(temp.substr(0,4) == "note")
            noteTagFound = true;
        else if(temp == "/part")
            return Note(-1, -1, "", "", false);
    }
    noteTagFound = false;

    string noteContents = "";
    while(noteContents.find("/note") == -1)
    {
        noteContents += input.get();
    }
    noteContents.erase(noteContents.end()-6, noteContents.end());

    return parseNote(noteContents);
}

void parseFile(string infile, string outfile)
{
    ifstream input(infile.c_str());
    ofstream output(outfile.c_str());

    int count = 0;
    Note n;
    bool eop = false; // end of part
    while(!eop)
    {
        count++;
        n = getNextNote(input);
        if(n.step == -1)
        {
            eop = true;
            break;
        }
        cout << "Note " << count << ": [";
        if(n.rest)
            cout << "REST";
        else
            cout << n.step << ", " << n.octave << ", " << n.alter;
        cout << ", " << n.type << "] --> ";
        cout << n.encode() << endl;

        output << n.encode() << "\n";
    }

    input.close();
    output.close();
}


int main(int argc, char *argv[])
{
    string infile = argv[1];
    string outfile = argv[2];

    parseFile(infile, outfile);

    return 0;
}

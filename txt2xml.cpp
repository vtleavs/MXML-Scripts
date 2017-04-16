#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
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

        int typeInt()
        {
            int t = 0;
            if(type == "whole")  t = 1;
            if(type == "half")  t = 2;
            if(type == "quarter")  t = 4;
            if(type == "eighth")  t = 8;
            if(type == "16th")  t = 16;
            if(type == "32nd")  t = 32;
            return t;
        }

        string encode()
        {
            int t = 0;
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
                case 'B': note = 3;
                case 'C': note = 4;
                case 'D': note = 6;
                case 'E': note = 8;
                case 'F': note = 9;
                case 'G': note = 11;
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

        void writeNote(ostream & output)
        {
            output << "<note>\n";

            if(rest)
            {
                output << "<rest/>\n";
            }
            else
            {
                output << "<pitch>\n";
                output << "<step>" << step << "</step>\n";
                if(alter != "0")
                    output << "<alter>" << alter << "</alter>\n";
                output << "<octave>" << (int)octave << "</octave>\n";
                output << "</pitch>\n";
            }

            float duration = 4.0/(float)typeInt();
            output << "<duration>" << duration << "</duration>\n";
            output << "<voice>1</voice>\n";
            output << "<type>" << type << "</type>\n";

            output << "</note>\n\n";
        }
};

Note parseNote(string data)
{
    string alter = "0";
    string type = "quarter";
    char step = -1;
    char octave = -1;
    bool rest = false;

    char c0 = data[0];
    char c1 = data[1];
    char c2 = data[2];
    char c3 = data[3];
    char c4 = data[4];

    string typeString({c1, c2});
    int typeInt = stoi(typeString);

    switch ( typeInt )
    {
        case 1: type = "whole";
            break;
        case 2: type = "half";
            break;
        case 4: type = "quarter";
            break;
        case 8: type = "eighth";
            break;
        case 16: type = "16th";
            break;
        case 32: type = "32nd";
            break;
        default: type = "quarter";
            break;
    }

    string st({c3, c4});
    int pitch = stoi(st);

    if(c0 == '1' || pitch < 1)
        rest = true;
    else
    {
        rest = false;

        octave = (pitch-1)/12;

        if(pitch%12 != 1 && pitch%12 != 3 && pitch%12 != 4 && pitch%12 != 6
            && pitch%12 != 8 && pitch%12 != 9 && pitch%12 != 11)
        {
            alter = "1";
            pitch--;
        }

        switch(pitch % 12)
        {
            case 1: step = 'A';
                break;
            case 3: step = 'B';
                break;
            case 4: step = 'C';
                break;
            case 6: step = 'D';
                break;
            case 8: step = 'E';
                break;
            case 9: step = 'F';
                break;
            case 11: step = 'G';
                break;
        }
    }

    Note n(step, octave, alter, type, rest);
    return n;
}

vector<Note> parseFile(string infile)
{
    ifstream input(infile.c_str());

    vector<Note> noteVec;

    int count = 0;
    Note n;
    bool eop = false; // end of part
    while(!input.eof())
    {
        string term;
        input >> term;
        count++;

        int length = term.length();

        if(length > 0)
        {

            if(length < 5)
            {
                cout << "*";
                for(int i = 0; i < 5-length; ++i)
                term += '0';
            }

            Note n = parseNote(term);

            cout << "Note " << count << ": " << term << " --> [";

            if(n.rest)
                cout << "REST";
            else
                cout << n.step << ", " << (int)n.octave << ", " << n.alter;
            cout << ", " << n.type << "]\n";
            noteVec.push_back(n);
        }

    }

    input.close();

    return noteVec;
}

void writeHeader(ostream & output)
{
    output << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<!DOCTYPE "
        "score-partwise PUBLIC \"-//Recordare//DTD MusicXML 3.0 Partwise//EN\" "
        "\"http://www.musicxml.org/dtds/partwise.dtd\">\n\n";
    output << "<score-partwise>\n";
    output << "<identification>\n"
        "<encoding>\n"
            "<software>MuseScore 2.0.3</software>\n"
            "<encoding-date>2017-04-05</encoding-date>\n"
            "<supports element=\"accidental\" type=\"yes\"/>\n"
            "<supports element=\"beam\" type=\"yes\"/>\n"
            "<supports element=\"print\" attribute=\"new-page\" type=\"yes\" value=\"yes\"/>\n"
            "<supports element=\"print\" attribute=\"new-system\" type=\"yes\" value=\"yes\"/>\n"
            "<supports element=\"stem\" type=\"yes\"/>\n"
        "</encoding>\n"
    "</identification>\n"
    "<defaults>\n"
        "<scaling>\n"
            "<millimeters>7.05556</millimeters>\n"
            "<tenths>40</tenths>\n"
        "</scaling>\n"
        "<page-layout>\n"
            "<page-height>1584</page-height>\n"
            "<page-width>1224</page-width>\n"
            "<page-margins type=\"even\">\n"
                "<left-margin>56.6929</left-margin>\n"
                "<right-margin>56.6929</right-margin>\n"
                "<top-margin>56.6929</top-margin>\n"
                "<bottom-margin>113.386</bottom-margin>\n"
            "</page-margins>\n"
            "<page-margins type=\"odd\">\n"
                "<left-margin>56.6929</left-margin>\n"
                "<right-margin>56.6929</right-margin>\n"
                "<top-margin>56.6929</top-margin>\n"
            "<bottom-margin>113.386</bottom-margin>\n"
            "</page-margins>\n"
        "</page-layout>\n"
        "<word-font font-family=\"FreeSerif\" font-size=\"10\"/>\n"
        "<lyric-font font-family=\"FreeSerif\" font-size=\"11\"/>\n"
    "</defaults>\n"
    "<part-list>\n"
        "<score-part id=\"P1\">\n"
            "<part-name>Piano</part-name>\n"
            "<part-abbreviation>Pno.</part-abbreviation>\n"
            "<score-instrument id=\"P1-I1\">\n"
                "<instrument-name>Piano</instrument-name>\n"
            "</score-instrument>\n"
            "<midi-device id=\"P1-I1\" port=\"1\"></midi-device>\n"
            "<midi-instrument id=\"P1-I1\">\n"
                "<midi-channel>1</midi-channel>\n"
                "<midi-program>1</midi-program>\n"
                "<volume>78.7402</volume>\n"
                "<pan>0</pan>\n"
            "</midi-instrument>\n"
        "</score-part>\n"
    "</part-list>\n\n"
    "<part id=\"P1\">\n";
}

void writeMeasureHeader(ostream & output, int beats)
{
    output << "<measure number=\"1\" width=\"251.47\">\n"
            "<print>\n"
                "<system-layout>\n"
                    "<system-margins>\n"
                        "<left-margin>0.00</left-margin>\n"
                        "<right-margin>0.00</right-margin>\n"
                    "</system-margins>\n"
                    "<top-system-distance>70.00</top-system-distance>\n"
                "</system-layout>\n"
            "</print>\n"
            "<attributes>\n"
                "<divisions>1</divisions>\n"
                "<key>\n"
                    "<fifths>0</fifths>\n"
                "</key>\n"
                "<time>\n"
                    "<beats>";
    output << beats << "</beats>\n"
                    "<beat-type>4</beat-type>\n"
                "</time>\n"
                "<clef>\n"
                    "<sign>G</sign>\n"
                    "<line>2</line>\n"
                "</clef>\n"
            "</attributes>\n\n";
}

void writeNote(ostream & output, Note n)
{
    output << "<note>\n";

    if(n.rest)
    {
        output << "<rest/>\n";
    }
    else
    {
        output << "<pitch>\n";
        output << "<step>" << n.step << "</step>\n";
        if(n.alter != "0")
            output << "<alter>" << n.alter << "</alter>\n";
        output << "<octave>" << (int)n.octave << "</octave>\n";
        output << "</pitch>\n";
    }

    float duration = 4.0/(float)n.typeInt();
    output << "<duration>" << duration << "</duration>\n";
    output << "<voice>1</voice>\n";
    output << "<type>" << n.type << "</type>\n";

    output << "</note>\n\n";
}

void writeFooter(ostream & output)
{
    output <<  "<barline location=\"right\">\n"
                "<bar-style>light-heavy</bar-style>\n"
                "</barline>\n"
                "</measure>\n"
                "</part>\n"
                "</score-partwise>\n";
}

void writeXMLFile(string outfile, std::vector<Note> notes)
{
    ofstream output(outfile.c_str());

    writeHeader(output);

    float count = 0;
    for( Note n : notes )
        count + (1.0/float(n.typeInt()));
    int beatCount = count + 1;

    writeMeasureHeader(output, beatCount);

    for( Note n : notes )
        n.writeNote(output);//writeNote(output, n);

    writeFooter(output);

    output.close();
}

int main(int argc, char *argv[])
{
    string infile = argv[1];
    string outfile = argv[2];

    vector<Note> vector = parseFile(infile);

    writeXMLFile(outfile, vector);

    return 0;
}

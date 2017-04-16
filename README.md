# MXML-Scripts
Scripts for encoding mxml data into 5 character numerical strings and back again

The purpose of these scripts are to create a standard representation of notes that is completely oblivious to syntax.  In its intended use, this is to ensure that when generated by an artifical intelligence the notes will always be valid.

xml2txt [input_file] [output_file]:
    This takes mxml data from input_file, which must have valid mxml formatting, and generates a five-digit numerical string for each note and stores them, separated by line in output_file.
    
    The data is formatting as shown:
        The form of the note code is abbcc, where a is a binary operator that represents whether it is a rest or not, bb represents the type of note such as quarter, half, etc, and cc is the pitch starting at A0.
        ex. 00439 -- this represents a quarter note with pitch B3.
        ex. 00841 -- this represents an eighth note with pitch C#3.
        ex. 10200 -- this represents a quarter note rest.
        
txt2xml [input_file] [output_file]:
    This does the inverse of xml2txt, decoding the generated numerical strings back into mxml format.  This has some limitations as the string does not include a great deal of information about the rest of the score.  Essentially this will put notes of a certain length and pitch into a single measure.  There will never be two notes played at the same time.
    The following are some quirks in the formatting:
        ex. 10200 -- this represents a quarter note rest.  The pitch is ignored if the first character is anything but 0.
        ex. 04539 -- this represents a quarter note with pitch B3; if any unsupported number is given for the note type, it defaults to a quarter note.
        ex. 90400 -- this represents a quarter note rest, as the first character is not 0.
        ex. 00200 -- thsi represents a half note with pitch A0, as an invalid pitch, in this case 0, defaults to A0.
        It is worth noting that any string of length > 5 that is given is truncated, and any string with length < 5 is automatically completed with 0's.
# Osomi String List (.oiSL)
This file format exists for the sole reason of storing encoded unique strings; it was first used by oiSH (SHader file format).  
The regular oiSL uses the default charset (space, 0-9, A-Z, a-z, .), if a char can't be found, it is automatically turned into a space.
# File specification
## Header
```cpp
struct SLHeader {

	char header[4];   //oiSL

	u8 version;       //SLHeaderVersion_s
	u8 perChar;
	u8 keys;
	u8 flags;         //SLHeaderFlags_s

	u16 names;
	u16 length;

};
```
'version' is the oiSL version; there's no new oiSL versions planned (Besides v1), but future updates could include unicode strings for example.  
'perChar' is the number of bits per character (default is 6).  
'keys' is the number of keys in the keyset. If this is 0 and the USE_DEFAULT (1) flag is checked, the default keyset is used.  
'flags' currently only has the 'default' flag, but could use more in the future.  
'names' is the string count.  
'length' is the character count.
## Keyset
The keyset is only present if it doesn't use the default keyset. Every character is flipped to at least secure the keyset from quick changes from notepad++ and other non-binary editors. The length of the keyset is sizeof(char) * keys (so char[keys], aka u8[keys]).
## Strings
Every string has a limit of 255 characters, this is because every string has a length as u8. Giving us u8[names].  
```
[ aaa, bbbb, cc ] with a default charset would be encoded as:
\x3\x4\x2 (aaabbbbcc)
```
Every element in u8[names] provides the length of a string, the offset increments after reading the characters from the encoded block (by that amount). This means that the 3rd string "cc" is stored at 3+4=7 in the encoded block; this means that with the default charset it is stored at bits `[42, 48>` (6 bits per char).
## Encoded block
As mentioned before, the encoded block is a bitset, allowing significant size reduction (especially for lots of strings with the same charset). Utf-8 compared to oiSL default is just 2 bits reduced, but compared to utf-16 it is 10. The 'perChar' field in the header defines how many bits one character takes up. This bitset is interpreted as a sized uint, that points to a key in the keyset.
# API usage
The wrapper around the raw structs is as following:
```cpp
struct SLFile {

	SLHeader header;
	String keyset;
	std::vector<String> names;

	u32 size;

	SLFile(String keyset, std::vector<String> names);
	SLFile();

	u32 lookup(String name);
	u32 add(String name);
	
}
```
You can create an SLFile by using the first constructor, or can use it for reading using the second cosntructor.
## Reading
An oiSL file can be read using a file path or buffer;
```cpp
SLFile file;
if(!oiSL::read("res/test.oiSL", file))
 ; //Handle error
```
Where "res/test.oiSL" follows the owc guidelines of a path. The path can also be subsituted for an actual buffer.  
You can then parse the strings given in the file's names variable and inspect the keyset.
## Writing
an oiSL file can be written to a Buffer or file using the following code;
```cpp
SLFile file(String::getDefaultCharset(), { "Hello", "World", "Is it me", "You are looking for" });
if(!oiSL::write(file, "out/test.oiSL"))
 ; //Handle error
 ```
 If no path is specified, it will return a Buffer instead. This buffer can be a null buffer if it couldn't write the file.

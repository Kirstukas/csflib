# CSF File format

1. Format begins with " FSC\x03"
2. Then three empty bytes
3. Four bytes identifying amount of strings
4. Four bytes identifying amount of strings substracted by one
5. Eight empty bytes

1. Five bytes indicating string name start " LBL\x01"
2. Three empty bytes
3. Four bytes indicating string name length
4. Name of tyhe string
5. Four bytes indicating string name stop " RTS"
6. Four bytes indicating string length
* Reversed char byte
* Byte including 0xFF

#### Lua script to get char codes
for i = 0, 255 do print(("%X  %c"):format(255-i, i)) end

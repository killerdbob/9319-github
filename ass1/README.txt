Hi, tutor

   this program use 'class tree' to create the huffman,
 I write for about 5 days to build from the base code, e.g.
, how to use bits class. this could make it easier for me to
reuse the code.
   the header store the tree, the maximum would only take 256*2+255*1+1=768(byte)
   1) the '256*2' is like 0000 0000 0000 0000, the first 8 bits if is 0, it means it is character, then it should read another 8 bits, if  the first 8 bits if is 1, then it is a node, we should combine the leave to a node and put it into priority queue. if the first 8 bits if is 2, then it is like 'EOF'. then I store 1 byte at the end of the header, this means how much bits is useless in the encoded code.
   2) I write several class to deal with this assignment, 'bitwise class', mainly deal with the bit operation. 'tree class', mainly to build a tree. 'huffuman class', this is for encoding and decoding and search, this helps in improving the coding beauty.
   thanks for giving such a good lecture.  

regards,
wei
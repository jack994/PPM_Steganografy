# PPM_Steganography

THE CODE FOR THIS PROJECT HAS BEEN ENCRYPTED DUE TO SIMILARITIES TO THE COURSEWORK OF THE CURRENT YEAR 2, TO HAVE ACCESS TO THE CODE SEND AN EMAIL TO gb11@hw.ac.uk

Steganography is a technique for hiding text in images, by replacing successive random pixels in the image with letters from the text. The text is then retrieved by comparing the new image with the old image, and extracting letters from the new one where it differs from the old one.

This program works with PPM images, some samples are in the 'P3 PPM' Folder.

--------------------------------------///\\\///\\\///\\\-------------------------------------------

For the call:

$ steg e file1.ppm > file2.ppm

steg prompts for a message and shows  file1.ppm with the message encoded within it. Here, the output is redirected to file2.ppm.

For the call:

$ steg d file1.ppm file2.ppm

steg shows the message hidden in file2.ppm after decoding by comparison with file1.ppm.

--------------------------------------///\\\///\\\///\\\-------------------------------------------

In the first part of the program I implemented the three structure needed to hold the PPM image and the functions to insert the image into the structure, encode and decode the image. I also have some helper function like the copyPPM() and the copy() function used to deepCopy the ppm structure and the linkedlist holding the comments into the structure.
The second part of the program is the main function which takes 2 or 3 arguments (which are 3 or 4 with the program itself) and depending on the number of arguments it encodes or decode the PPM file passed as an argument the first argument has to be an “e” if we want to encode the File or a “d” if we want to decode.

Choices:
-	I chose to use a linkedList for the comments because I thought it would have been easier to store them in this way as we don’t know how many comment lines we have at the moment of reading the file, in this way I can go through the file with the file pointer and add a node with a comment to the Linked list every time I encounter an “#” and stop when the subsequent line does not start with it.
-	I chose to use an array of pixels to hold the image because it was the easiest and fastest way in my opinion, every pixel holds 3 values (red, green, blue) and having an array of the structure PIXEL was really handy and easy to work with.
-	I chose to encode the secret message using the ASCII character cast as an integer so that replacing the value of the red field in the pixel never gives any error as both maximum is 255.
I also use a random number (with a maximum used as a modulo) generated every loop cycle to insert the ASCII value in a random position in the image.

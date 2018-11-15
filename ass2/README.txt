Hi tutor,
	this assignment is write by c++.
	implementation:	
	1)bwtencode, I first search every letter in the file by  ascii order, and I sort the letter by its alphabetically order by bucket sort. Then, I write the result to the file.
	The index file, I store the position information of the delimeter, this is helpful in search
	2)bwtsearch, I store a checkpoint of the character numbers for every 1000 letters. And I sorted the encoded file by ascii order, this could get the First array and I only store the first position of the letter in order to save memory.
	For '-i', '-a', '-m' or '-n', these could be finish by backward search, however, this need to read the encoded file several time, which may cause high IO. The previous index file which could help to reduce high IO and increase speed.
regards,
wei 
//Librairies standardes
#include <stdlib.h>
#include <stdio.h>

//Fonctions POSIX
#include <sys/stat.h>

#define USAGE "<input_raw_file> rowsize colsize depth header <output_pgm_file>"
#define SIZE_BUFFER 65536

int32_t main(int32_t argc, char *argv[])
{
	FILE *input, *output;
	u_int64_t rs, cs, d, size, read, h, count;
	struct stat file_status;
	char tab[SIZE_BUFFER];

	//Check inputs outputs
	if(argc!=7)
	{
		fprintf(stderr, "usage: %s %s\n", argv[0], USAGE);
		return(-1);
	}

	input=fopen(argv[1], "rb");
	if(input==NULL)
	{
		fprintf(stderr, "Error: Input file %s not found.\n", argv[1]);
		return(-1);
	}

	size=0;
	if(fstat(fileno(input), &file_status) != 0)
	{
		fprintf(stdout, "Warning: Input file is very large, cannot make any preprocessing on header size.\n");
		size=1;
	}

	rs=atoi(argv[2]);
	cs=atoi(argv[3]);
	d=atoi(argv[4]);
	h=atoi(argv[5]);
	if(size==0)
		size=file_status.st_size;
	else
		size=cs*rs*d;

	if(rs<0 || cs<0 || d<0 || h<0)
	{
		fprintf(stderr, "Error: Image cannot have negative dimensions or header.\n");
		return(-1);
	}

	if(rs*cs*d < size)
	{
		fprintf(stdout, "Warning: Provided sizes do not match file size in memory. File might have header which will be skipped (%ju bytes).\n", size-(rs*cs*d));
	}
	else if(rs*cs*d > size)
	{
		fprintf(stderr, "Error: Provided dimensions give a resulting file size (%ju bytes) higher than original file size (%ju bytes).\n", rs*cs*d, size);
		return(-1);
	}

	output=fopen(argv[6], "wb");
	if(output==NULL)
	{
		fprintf(stderr, "Error: Output file %s could not be created... check directory access.\n", argv[5]);
		return(-1);
	}


	//We read in input and write in output
	count=0;
	fprintf(output, "P5\n%ju %ju %ju\n255\n", rs, cs, d);
	fprintf(stdout, "Processing image...     ");
	fflush(stdout);
	fseek(input, h, SEEK_SET);

	while(!feof(input))
	{
		read=fread(tab, sizeof(char), SIZE_BUFFER, input);
		count+=read;
		fwrite(tab, sizeof(char), read, output);
		fprintf(stdout, "\b\b\b\b");
		fprintf(stdout, "%3u%c", (u_int32_t)((double)count/(double)size*100.0), '%');
		fflush(stdout);
		fflush(output);
	}

	fprintf(stdout, "\b\b\b\bdone\n");

	//We check if we read exactly the goo amount of bytes
	if(count-size < 0)
	{
		fprintf(stdout, "Warning: The program did not read enough bytes (Read: %ju, Expected: %ju)...\nAre you sure the provided dimensions are not too big?", count, size);
	}
	else if(count-size > 0)
	{
		fprintf(stdout, "Warning: The program read too much bytes (Read: %ju, Expected: %ju)...\nMaybe the file has a header of %ju bytes.\n", count, size, count-size);
	}

	fclose(input);
	fclose(output);
	return(0);
}





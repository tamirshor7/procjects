#include <stdio.h>

int main()
{
	FILE* f;
	FILE* fr;
	char buf [200];


	char namestring [7] = {'t','e','s','t','?','?','\0'};
	for(int i=0; i<3; i++)
	{
		for(int j=0;j<9;j++)
		{
			namestring[4] = (char)(i+'0');
			namestring[5] = (char)(j+'0');
			f = fopen(namestring,"w");
			fr = fopen("test.html","r");
			char* str = fgets(buf,100,(FILE*) fr);
			while(fr && str[0]!='\n')
			{
				
				if(str[1] == 't')
				{
					fprintf(f,"<title>OS-HW3 Test Web Page %d%d</title>\n",i,j);
				}
				else
				{
					fputs(str,(FILE*)f);

				}
				char* str = fgets(buf,100,(FILE*) fr);

			}
			
			fclose(f);
			fclose(fr);
		}

		

	}
	
}
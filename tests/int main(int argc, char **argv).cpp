int main(int argc, char **argv)
{
	setlocale(LC_ALL, ""); // support unicode ! (é, à, è etc.)
 
	printf("------------------------\n");
	printf("--------- µOCR ---------\n");
	printf("----------v2.7----------\n");
	printf("------(26/01/2013)------\n");
	printf("------------------------\n");
    
    int fileExists = 0;

    if (argc > 1) {
        strcpy(nomFichier, argv[1]);
        printf("Chargement de l'image : %s\n", nomFichier);
    } else {
        char input[50];
        input[0] = '\0';
        
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"  // shut up gcc (warning: ignoring return value of ‘scanf’, declared with attribute warn_unused_result [-Wunused-result])
												  // a cause du -O3.   L'astuce de caster le return en void ne marche pas(/plus?) apparement.
        while (strlen(input)<1) { printf("Please type the name of the file to load: \n"); fgets(input, sizeof(input), stdin); //replaced gets(input) with fgets to prevent overflow but mostly just the warning
#pragma GCC diagnostic pop
        printf("\n");
        strcpy(nomFichier, input);
    }
    
    if (strEndsWith(nomFichier,".jpg") || strEndsWith(nomFichier,".gif") || strEndsWith(nomFichier,".png") || strEndsWith(nomFichier,".tif") || strEndsWith(nomFichier,".tiff")) {
			printf("Only BMP (24 bit) images are supported. An automatic conversion will be attempted (via GraphicsMagick).\n");
			
			char cmd[100];
			sprintf(cmd, "convert %s %s.bmp", nomFichier, nomFichier);
			
			if (system(cmd) == 0) {
				printf("The conversion was successful. The new file was saved as %s.bmp. Loading it.\n", nomFichier);
				strcat(nomFichier, ".bmp");
			} else {
				printf("The conversion has failed, please convert your file manually and try again.\n");
				return -1;
			}
	} else if (!strEndsWith(nomFichier,".bmp")) {
		strcat(nomFichier, ".bmp");
	}
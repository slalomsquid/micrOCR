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
        while (strlen(input)<1) { printf("Veuillez taper le nom du fichier à charger : \n"); fgets(input, sizeof(input), stdin);; }			      
#pragma GCC diagnostic pop
        printf("\n");
        strcpy(nomFichier, input);
    }
    
    if (strEndsWith(nomFichier,".jpg") || strEndsWith(nomFichier,".gif") || strEndsWith(nomFichier,".png") || strEndsWith(nomFichier,".tif") || strEndsWith(nomFichier,".tiff")) {
			printf("Seules les images BMP (24 bit) sont supportées. Une conversion automatique va être tentée (via GraphicsMagick).\n");
			
			char cmd[100];
			sprintf(cmd, "convert %s %s.bmp", nomFichier, nomFichier);
			
			if (system(cmd) == 0) {
				printf("La conversion a réussie. Le nouveau fichier a été sauvé en tant que %s.bmp. Chargement de celui-ci.\n", nomFichier);
				strcat(nomFichier, ".bmp");
			} else {
				printf("La conversion a écouchée, veuillez convertir votre fichier manuellement et réessayer..\n");
				return -1;
			}
	} else if (!strEndsWith(nomFichier,".bmp")) {
		strcat(nomFichier, ".bmp");
	}
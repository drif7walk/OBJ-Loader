/* Deep magic starts here... */
void ReadOBJ(char* filename, Node* node)
{
	if (node->vertices != NULL) free(node->vertices);
	if (node->vertexcount != 0) node->vertexcount = 0;
	if (node->normals != NULL) free(node->normals);
	if (node->texcoords != NULL) free(node->texcoords);

	node->vertices = malloc(0);
	node->normals = malloc(0);
	node->texcoords = malloc(0);

	FILE *fp = fopen(filename, "r");

	char buf[256];

	int numfaces = 0;

	float* positions = malloc(0); int numpositions = 0;
	float* uvs = malloc(0); int numuvs = 0;
	float* norms = malloc(0); int numnorms = 0;
	while (fgets (buf, sizeof(buf), fp)) {

			char buf2[8];
			float x, y, z;
			int r=sscanf(buf, "%s %f %f %f", buf2, &x, &y, &z);

			if (strcmp(buf2, "v") == 0)
			{
				positions = realloc(positions, (numpositions+1)*3*sizeof(float));

				positions[numpositions*3] = x;
				positions[numpositions*3+1] = y;
				positions[numpositions*3+2] = z;

				numpositions++;
			}

			if (strcmp(buf2, "vt") == 0)
			{
				float nx, ny;
				sscanf(buf, "vt %f %f", &nx, &ny);
				uvs = realloc(uvs, (numuvs+1)*2*sizeof(float));
				uvs[numuvs*2] = nx;
				uvs[numuvs*2+1] = ny;

				numuvs++;
			}


			if (strcmp(buf2, "vn") == 0)
			{
				float nx, ny, nz;
				norms = realloc(norms, (numnorms+1)*3*sizeof(float));
				int r = sscanf(buf, "vn %f %f %f", &nx, &ny, &nz);
				norms[numnorms*3] = nx;
				norms[numnorms*3+1] = ny;
				norms[numnorms*3+2] = nz;

				numnorms++;
			}

			/* Make indices from face data and hope that it works */
			if (strcmp(buf2, "f")==0)
			{
				int vn1, vn2, vn3;
				int f[3];

				int t1, t2, t3;
				int r = sscanf(buf, "f %i/%i/%i %i/%i/%i %i/%i/%i", &f[0], &t1, &vn1, &f[1], &t2, &vn2, &f[2], &t3, &vn3);

				for (int i = 0; i < 3; i++) f[i]--;
				vn1--; vn2--; vn3--;
				t1--;t2--;t3--;


				node->vertices = realloc(node->vertices, (numfaces+1)*3*3*sizeof(float));
				for (int i = 0; i < 9; i++)
				{
					node->vertices[node->vertexcount*3+i] = positions[f[(int)floor(i/3)]*3+(i%3)];
				}

				node->vertexcount += 3;

				node->texcoords = realloc(node->texcoords, (numfaces+1)*3*2*sizeof(float));
				node->texcoords[numfaces*3*2] = uvs[ t1*2 ];
				node->texcoords[numfaces*3*2+1] = uvs[ t1*2+1 ];

				node->texcoords[numfaces*3*2+2] = uvs[ t2*2 ];
				node->texcoords[numfaces*3*2+3] = uvs[ t2*2+1 ];

				node->texcoords[numfaces*3*2+4] = uvs[ t3*2 ];
				node->texcoords[numfaces*3*2+5] = uvs[ t3*2+1 ];

				node->normals = realloc(node->normals, (numfaces+1)*3*3*sizeof(float));
				node->normals[numfaces*3*3] = norms[ vn1*3 ];
				node->normals[numfaces*3*3+1] = norms[ vn1*3 + 1];
				node->normals[numfaces*3*3+2] = norms[ vn1*3 + 2 ];
				node->normals[numfaces*3*3+3] = norms[ vn2*3 ];
				node->normals[numfaces*3*3+4] = norms[ vn2*3 + 1];
				node->normals[numfaces*3*3+5] = norms[ vn2*3 + 2 ];
				node->normals[numfaces*3*3+6] = norms[ vn3*3 ];
				node->normals[numfaces*3*3+7] = norms[ vn3*3 + 1];
				node->normals[numfaces*3*3+8] = norms[ vn3*3 + 2 ];

				numfaces++;
			}

			if (strcmp(buf2, "mtllib") == 0)
			{
				int space = strchr(buf, ' ') - buf;
				strcpy(buf2, buf+space+1);
				buf2[strlen(buf2)-1] = 0;

				char mtlpath[8] = "./";
				char mtlname[24];
				sprintf(mtlname, "%s%s", mtlpath, buf2);
                printf("Material: %s\r\n", mtlname);

				// Obtained a material file
				FILE * mtl = fopen(mtlname, "r");
				if (mtl == NULL) printf ("Cannot read %s\r\n", mtlname);
				while (fgets (buf, sizeof(buf), mtl)) {
						r = sscanf(buf, "%s %f %f %f", buf2, &x, &y, &z);

						if (strcmp(buf2, "map_Kd")==0)
						{
							sscanf(buf, "map_Kd %s", buf2); // Obtained the texture filename
							memset(mtlname, 0, sizeof(mtlname));
							sprintf(mtlname, "%s%s", mtlpath, buf2); // Repurposing mtlname to bitmap name

							// Finally obtained the texture
							node->texture[0] = LoadBMP24Tex(mtlname);

						}
				}

				fclose(mtl);
			}
	}
	if (ferror(fp)) {
		fprintf(stderr,"Oops, error reading stdin\n");
		abort();
	}

	free(positions);
	free(uvs);
	free(norms);

	fclose(fp);

	printf("%s\r\n", filename);
	printf("%i vertices.\r\n", node->vertexcount);
}

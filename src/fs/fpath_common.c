

const char* get_file_name(const char *path)
{
	int i;

	if (path[strlen(path)] == '/')
		return NULL;

	for (i=strlen(path)-1;i>0;i--)
		if (path[i] == '/')
			return &(path[i+1]);

	return NULL;
}


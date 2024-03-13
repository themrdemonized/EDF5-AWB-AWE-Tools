// AWB_Repacker, author unknown, grabbed from https://steamcommunity.com/sharedfiles/filedetails/?id=632355452

// edit by demonized

// 1. Fixed code to be compiled with the latest GCC. Compiled into 64 bit exe.
// To compile the app: gcc AWB_repacker.c

// 2. Support for folder argument to process all files in the folder, usage: AWB_repacker.exe path_to_folder

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h> 

#define VERBOSE_PRINT

void write_file(uint8_t* data, uint32_t size) {
	FILE* file;
	uint32_t	ret;

	file = fopen("OUT.AWB", "wb");
	if (file == NULL) {
		printf("unable to create new file\n");
		exit(EXIT_FAILURE);
	}
	ret = fwrite(data, sizeof(*data), size, file);
	if (ret != size) {
		printf("an unknown error may have occurred\n");
		exit(EXIT_FAILURE);
	}

	fclose(file);

#ifdef VERBOSE_PRINT
	printf("archive successfully created (%d bytes long)\n", size);
#endif // VERBOSE_PRINT

	return;
}

uint32_t* get_files_size(char** files, uint32_t fileNumber) {
	uint32_t* size;
	uint32_t	i;
	struct _stat	buf;

	size = malloc(fileNumber * sizeof(*size));
	for (i = 0; i < fileNumber; i++) {

		int fstat = _stat(files[i], &buf);
		int fstatRes = fstat != 0;

#ifdef VERBOSE_PRINT
		printf("[%d / %d] %s opened, size %d\n", i + 1, fileNumber, files[i], buf.st_size);
#endif

		if (fstatRes) {
			printf("unable to retrieve information about %s, error %d, errno %d\n", files[i], fstat, errno);
			exit(EXIT_FAILURE);
		}

		size[i] = buf.st_size;
	}

	return (size);
}

uint32_t create_header(uint8_t** header, uint32_t fileNumber) {
	int8_t		dummy[] = { 0x41, 0x46, 0x53, 0x32, 0x01, 0x04, 0x02, 0x00 };
	int32_t		alignment = 32;
	uint16_t	fileIndex[fileNumber];
	uint32_t	headerSize;
	uint8_t* offset;
	uint32_t	i;

	for (i = 0; i < fileNumber; i++) {
		fileIndex[i] = i;
	}

	headerSize = 16 + sizeof(fileIndex) + ((fileNumber + 1) * 4);
	headerSize += 31 - ((headerSize - 1) % 32);

	*header = calloc(headerSize, sizeof(*header));

	offset = *header;
	memcpy(offset, dummy, sizeof(dummy));
	offset += sizeof(dummy);
	memcpy(offset, &fileNumber, sizeof(fileNumber));
	offset += sizeof(fileNumber);
	memcpy(offset, &alignment, sizeof(alignment));
	offset += sizeof(alignment);
	for (i = 0; i < fileNumber; i++) {
		memcpy(offset, &fileIndex[i], sizeof(fileIndex[i]));
		offset += sizeof(fileIndex[i]);
	}

#ifdef VERBOSE_PRINT
	printf("header generated (%d bytes long)\n", headerSize);
#endif // VERBOSE_PRINT

	return (headerSize);
}

void add_offsets(uint8_t* header, uint32_t headerSize, uint32_t* fileSizeTab, int fileNumber) {
	uint32_t	offset;
	int			i;

	offset = 16 + (fileNumber * 2);
	headerSize = 16 + (fileNumber * 2) + ((fileNumber + 1) * 4);

	for (i = 0; i < fileNumber; i++) {
		memcpy(header + offset, &headerSize, 4);
		headerSize += 31 - ((headerSize - 1) % 32);
		headerSize += fileSizeTab[i];
		offset += 4;
	}
	memcpy(header + offset, &headerSize, 4);

	return;
}

uint32_t add_file(uint8_t** data, char* path, uint32_t dataSize, uint32_t fileSize, int fileIndex, int fileNumber) {
	FILE* stream;
	uint8_t* buf;
	uint8_t* newfile;
	int			ret;
	int			padding;

	stream = fopen(path, "rb");
	if (stream == NULL) {
		printf("unable to open %s\n", path);
		exit(EXIT_FAILURE);
	}

	padding = 31 - ((fileSize - 1) % 32);
	buf = calloc(fileSize + padding, sizeof(*buf));

	ret = fread(buf, 1, fileSize, stream);
	if (ret == -1) {
		printf("unable to read %s\n", path);
		exit(EXIT_FAILURE);
	}

	if (ret != (int)fileSize) {
		printf("unknown error while reading %s\n", path);
		exit(EXIT_FAILURE);
	}

	newfile = malloc((dataSize + fileSize + padding) * sizeof(*newfile));
	memcpy(newfile, *data, dataSize);
	memcpy(newfile + dataSize, buf, fileSize + padding);

	fclose(stream);
	free(*data);
	free(buf);

	*data = newfile;

#ifdef VERBOSE_PRINT
	printf("[%d / %d] %s added successfully (%d bytes long)\n", fileIndex, fileNumber, path, fileSize);
#endif // VERBOSE_PRINT

	return (dataSize + fileSize + padding);
}

int isDirectory(const char* path) {
	struct stat statbuf;
	if (stat(path, &statbuf) != 0)
		return 0;
	return S_ISDIR(statbuf.st_mode);
}

int	main(int argc, char** argv) {
	uint8_t* data;
	uint32_t	dSize;
	uint32_t* fileSizeTab;
	int			i;

	if (argc == 1) {
		printf("usage: %s <files | directory>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (isDirectory(argv[1])) {
		printf("%s is directory\n", argv[1]);
		DIR* d;
		struct dirent* dir;
		d = opendir(argv[1]);
		if (d) {
			int count = 0;

			while ((dir = readdir(d)) != NULL) {
				char* name = dir->d_name;
				if (strstr(name, ".bin") != NULL) {
					count++;
				}
			}
			closedir(d);

#ifdef VERBOSE_PRINT
			printf("%d files in directory, allocating\n", count);
#endif

			d = opendir(argv[1]);

			int size = count + 1;
			count = 1;
			char** args = malloc(size * 8);

			args[0] = malloc(strlen(argv[0]) + 1);
			strcpy(args[0], argv[0]);

			while ((dir = readdir(d)) != NULL) {
				char name[500] = "";
				//printf("new name %s\n", name);

				strcat_s(name, _countof(name), argv[1]);
				if (argv[1][strlen(argv[1]) - 1] != '\\') {
					strcat_s(name, _countof(name), "\\");
				}
				strcat_s(name, _countof(name), dir->d_name);

				if (strstr(name, ".bin") != NULL) {
					args[count] = malloc(strlen(name) + 1);

					//#ifdef VERBOSE_PRINT
					//					printf("%d: %s\n", count, name);
					//#endif

					strcpy(args[count], name);
					count++;
				}
			}
			closedir(d);

			//#ifdef VERBOSE_PRINT
			//			for (int i = 0; i < count; i++) {
			//				printf("%s\n", args[i]);
			//			}
			//#endif

			dSize = create_header(&data, count - 1);
			fileSizeTab = get_files_size(args + 1, count - 1);
			add_offsets(data, dSize, fileSizeTab, count - 1);
			for (i = 0; i + 1 < count; i++) {
				dSize = add_file(&data, args[i + 1], dSize, fileSizeTab[i], i + 1, count - 1);
			}
			write_file(data, dSize);
			free(data);
			free(fileSizeTab);

			for (int i = 0; i < count; i++) {
				free(args[i]);
			}
			free(args);

			return (EXIT_SUCCESS);
		} else {
			printf("the argument %s neither a file nor a directory\n", argv[1]);
			exit(EXIT_FAILURE);
		}
	} else {
		dSize = create_header(&data, argc - 1);
		fileSizeTab = get_files_size(argv + 1, argc - 1);
		add_offsets(data, dSize, fileSizeTab, argc - 1);
		for (i = 0; i + 1 < argc; i++) {
			dSize = add_file(&data, argv[i + 1], dSize, fileSizeTab[i], 1, 1);
		}
		write_file(data, dSize);
		free(data);
		free(fileSizeTab);

		return (EXIT_SUCCESS);
	}
}

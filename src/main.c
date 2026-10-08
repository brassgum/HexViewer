#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

const char* g_HEX = "0123456789ABCDEF";
const int g_PADDING = 6;

typedef enum
{
	PMODE_HEX = 1
	, PMODE_TEXT = 2
	, PMODE_DUAL = 3
} pmode_t;

typedef struct
{
	int line;
	int byte_align;
	pmode_t mode;
	int size;
	char* buffer;
} print_block_t;

void byte_to_hex(char* out_buffer, const char* data, int length);
void byte_to_text(char* out_buffer, const char* data, int length);
void print_data(const print_block_t* print_block, const char* data, int length);

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		printf("Usage: ./HexViewer file\n");
		return 0;
	}

	FILE* file = fopen(argv[1], "rb");
	if (file == NULL)
	{
		printf("File cannot find\n");
		return 0;
	}
	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* content = malloc(file_size);
	assert(content != NULL);

	fread(content, 1, file_size, file);

	fclose(file);

	
	print_block_t print_block;
	print_block.line = 20;
	print_block.byte_align = 0x10;
	print_block.mode = PMODE_DUAL;
	print_block.size = print_block.byte_align * 4 + g_PADDING * 2;
	print_block.buffer = malloc(sizeof(char) * print_block.size);
	assert(print_block.buffer != NULL);

	int offset = 0;
	int command_buffer[4];
	char* command = (char*)&command_buffer;

	while (1)
	{
		system("clear");
		printf("offset: %d\n", offset);
		print_data(&print_block, content + offset, file_size - offset);

		printf("set 'o'ffset, 'q'uit, change 'a'lign, set show 'l'ine, 'd'own, 'u'p\n");
		scanf("%s", command);
		switch (*command)
		{
		case 'o': // set offset
			scanf("%d", &offset);
			break;
		case 'q': // quit
			goto lb_shutdown;
		case 'a':
			scanf("%d", &print_block.byte_align);
			free(print_block.buffer);
			print_block.size = print_block.byte_align * 4 + g_PADDING * 2;
			print_block.buffer = malloc(sizeof(char) * print_block.size);
			assert(print_block.buffer != NULL);
			break;
		case 'l':
			scanf("%d", &print_block.line);
			break;
		case 'd':
			offset += print_block.byte_align;
			break;
		case 'u':
			offset -= print_block.byte_align;
			if (offset < 0)
			{
				offset = 0;
			}
			break;
		default:
			break;
		}
	}

lb_shutdown:

	free(print_block.buffer);
	free(content);

	return 0;
}

void byte_to_hex(char* out_buffer, const char* data, int length)
{
	for (int i = 0; i < length; ++i)
	{
		int high = data[i] & 0xf0;
		high = high >> 4;
		int low = data[i] & 0xf;
		out_buffer[3 * i] = g_HEX[high];
		out_buffer[3 * i + 1] = g_HEX[low];
		out_buffer[3 * i + 2] = 0x20;
	}

	return;
}

void byte_to_text(char* out_buffer, const char* data, int length)
{
	for (int i = 0; i < length; ++i)
	{
		if (data[i] < 0x7f && 0x10 < data[i])
		{
			out_buffer[i] = data[i];
		}
		else
		{
			out_buffer[i] = '.';
		}
	}
	return;
}

void print_data(const print_block_t* print_block, const char* data, int length)
{
	int offset = 0;
	for (int i = 0; i < print_block->line; ++i)
	{
		for (int j = 0; j < print_block->size; ++j)
		{
			print_block->buffer[j] = 0x20;
		}

		int col = length - offset;

		if (col <= 0)
		{
			break;
		}

		if (print_block->byte_align < col)
		{
			col = print_block->byte_align;
		}

		switch (print_block->mode)
		{
		case PMODE_HEX:
			byte_to_hex(print_block->buffer, data + offset, col);
			break;
		case PMODE_TEXT:
			byte_to_text(print_block->buffer, data + offset, col);
			break;
		case PMODE_DUAL:
			byte_to_hex(print_block->buffer, data + offset, col);
			byte_to_text(print_block->buffer + col * 3 + g_PADDING, data + offset, col);
			break;
		default:
			assert(0);
		}

		printf("+%d\t", offset);
		fwrite(print_block->buffer, 1, print_block->size, stdout);
		printf("\n");

		offset += col;
	}

	return;
}


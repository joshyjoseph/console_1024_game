#include<string.h>
#include<time.h>
#include<ncurses.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#define init_x 45
#define init_y 8

#define max_x 93
#define max_y 29

#define x_diff 12
#define y_diff 5

int table[4][4];
int has_result = 0;
char *result = NULL;

int target = 1024;
FILE *fp;

void init_table(void);

/*********************** misc section *****************************/

void print_message(char *st)
{
	move(init_y-1, init_x);
	addstr(st);
}

/*********************** file access section *****************************/

int inline file_exist(char *fname)
{
	return access( fname, F_OK ) != -1;
}

void save_table(char *fname)
{
	int i, j;
	fp = fopen(fname, "w");
	fprintf(fp, "%d ", target);
	for(i=0; i<4; i++)
	{
		for(j=0; j<4; j++)
		{
			fprintf(fp, "%d ", table[i][j]);
		}
	}
	fclose(fp);
}

int is_power_of_two(int n)
{
	int i;
	if( n == 0 ) return 1;
	for( i = 2; i < 4096; i = i << 1 )
	{
		if( n == i )
			return 1;
		if( n < i )
			return 0;
	}
	return 0;
}

void read_from_file(char *fname)
{
	int i, j, c;
	fp = fopen(fname, "r");
	fscanf(fp, "%d", &target);
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)
		{
			c = fscanf(fp, "%d", &table[i][j]);
			if(!is_power_of_two(table[i][j]))
			{
				print_message("Reseting....");
				init_table();
				save_table(fname);
				return;
			}
		}
	}	
	fclose(fp);
}

/*********************** graphics section *****************************/

void draw_vertical_lines(int x, int y_start, int y_end)
{
	int i = 0, y = y_start;
	int hight = y_end - y_start;
	for(i = 0; i < hight; i++)
	{
		move(y, x);
		if(i%y_diff == 0)
			addstr("+");
		else
			addstr("|");
		y++;
	}
}

void draw_horizondal_lines(int y, int x_start, int x_end)
{
	int i = 0, x = x_start;
	int width = x_end - x_start;
	for(i = 0; i < width; i++)
	{
		move(y, x);
		addstr("-");
		x++;
	}
}

void draw_table(int n)
{
	int i, j;	
	int x = init_x;	
	float y = init_y;
		
	for(i = 0; i < n; i++)
	{
		draw_horizondal_lines(y, init_x, max_x);
		y += y_diff;
	}	
	
	x = init_x;
	for(i = 0; i < n; i++)
	{
		draw_vertical_lines(x, init_y, max_y);
		x += x_diff;
	}	
	
}

/*********************** matrix section *****************************/

void inline swap(int *a, int *b)
{
	int t = *a;
	*a = *b;
	*b = t;
}

void clear_value(int i, int j)
{
	swap(&i, &j);
	int x=0, y=0;
	x = init_x + (i+1)*(x_diff) - (x_diff/2);
	y = init_y + (j+1)*(y_diff) - (y_diff/2);
	move(y, x-2);
	addstr("     ");
}

void set_value(int i, int j, int val)
{
	int x=0, y=0;
	char   buffer[10];
	
	swap(&i, &j);
	x = init_x + (i+1)*(x_diff) - (x_diff/2);
	y = init_y + (j+1)*(y_diff) - (y_diff/2);
	if(val > 100)
		move(y, x-1);
	else
		move(y, x);
	sprintf(buffer, "%d", val);
	addstr(buffer);	
}

void update_table()
{
	int i,j;
	for(i = 0; i < 4; i++)	
	{
		for(j = 0; j < 4; j++)
		{
			clear_value(i, j);	
			if(table[i][j] == 0) 
				continue;			
			set_value(i, j, table[i][j]);
		}
	}
}

int set_random_unused_entry()
{
	static int px[2];
	static int py[2];
	int x, y;
	srand ( time(NULL) );
	
	while(1)
	{
		x = rand() % 4;
		y = rand() % 4;
		if(table[x][y] != 0)
			continue;
		if((x == px[0] && y == py[0]) || ( x == px[1] && y == py[1] ))
			continue;
		break;
	}
	px[0] = px[1];
	py[0] = py[1];
	px[1] = x;
	py[1] = y;
	table[x][y] = 2;
}

int check_result(int *c)
{
	int i, j, cnt = 0;
	for( i = 0; i < 4; i++)
	{
		for( j = 0; j < 4; j++)
		{
			if(table[i][j] == 0)
			{
				++cnt;
			}
			else if(table[i][j] == target)
			{
				result = "You Win... Press Enter to Exit";
				has_result = 1;
				return 1;
			}			
		}
	}
	if( cnt == 0 )
	{
		for(i = 0; i < 4; i++)
		{
			for(j = 0; j < 3; j++)
			{
				if( table[i][j] == table[i][j+1])
					return 0;
			}
		}
		
		for(j = 0; j < 4; j++)
		{
			for(i = 0; i < 3; i++)
			{
				if( table[i][j] == table[i+1][j])
					return 0;
			}
		}
		
		result = "You LOST... Press Enter to Exit";
		has_result = 1;
		return -1;
	}
	*c = cnt;
	return 0;
}

void set_random_entry()
{
	int cnt = 0;
	int i, j;
	if(check_result(&cnt) == 0)
	{
		if(cnt > 2)
		{
			set_random_unused_entry();
		}
		else
		{
			for( i = 0; i < 4; i++)
			{
				for( j = 0; j < 4; j++)
				{
					if(table[i][j] == 0)
					{
						table[i][j] = 2;
						return;
					}
				}
			}
		}
	}
}

void init_table()
{
	
	int i, j;
	for(i=0; i<4; i++)
	{
		for(j=0; j<4; j++)
		{
			table[i][j] = 0;
		}
	}
	
	srand ( time(NULL) );
	int x1, x2, y1, y2;
	while(1)
	{
		x1 = rand() % 4;
		x2 = rand() % 4;
		y1 = rand() % 4;
		y2 = rand() % 4;
		
		if(x1 == x2 && y1 == y2)
			continue;
		break;
	}
	table[x1][y1] = 2;
	table[x2][y2] = 2;
}

/*********************** movement section *****************************/

void shift_up(int c)
{
	int z = 0;
	int i = 0;
	for(i = 0; i < 4; i++)
	{
		if(i == z && table[i][c] != 0)
		{
			z++;
			continue;
		}
		if(table[i][c] == 0)
		{
			continue;
		}
		table[z++][c] = table[i][c];
		table[i][c] = 0;
	}
}

void shift_down(int c)
{
	int z = 3;
	int i = 3;
	for(i = 3; i >= 0; i--)
	{
		if(i == z && table[i][c] != 0)
		{
			z--;
			continue;
		}
		if(table[i][c] == 0)
		{
			continue;
		}
		table[z--][c] = table[i][c];
		table[i][c] = 0;
	}
}

void shift_left(int r)
{
	int z = 0;
	int i = 0;
	for(i = 0; i < 4; i++)
	{
		if(i == z && table[r][i] != 0)
		{
			z++;
			continue;
		}
		if(table[r][i] == 0)
		{
			continue;
		}
		table[r][z++] = table[r][i];
		table[r][i] = 0;
	}
}

void shift_right(int r)
{
	int z = 3;
	int i = 3;
	for(i = 3; i >= 0; i--)
	{
		if(i == z && table[r][i] != 0)
		{
			z--;
			continue;
		}
		if(table[r][i] == 0)
		{
			continue;
		}
		table[r][z--] = table[r][i];
		table[r][i] = 0;
	}
}

void add_up(int c)
{
	int i;
	for(i = 0; i < 3; i++)
	{
		if(table[i][c] == table[i+1][c])
		{
			table[i][c] *= 2;
			table[++i][c] = 0;
		}
	}
}

void add_down(int c)
{
	int i;
	for(i = 3; i > 0; i--)
	{
		if(table[i][c] == table[i-1][c])
		{
			table[i][c] *= 2;
			table[--i][c] = 0;
		}
	}
}

void add_right(int r)
{
	int i;
	for(i = 3; i > 0; i--)
	{
		if(table[r][i] == table[r][i-1])
		{
			table[r][i] *= 2;
			table[r][--i] = 0;
		}
	}
}

void add_left(int r)
{
	int i;
	for(i = 0; i < 3; i++)
	{
		if(table[r][i] == table[r][i+1])
		{
			table[r][i] *= 2;
			table[r][++i] = 0;
		}
	}
}

void move_up()
{
	int i;
	for(i = 0; i < 4; i++)
	{
		shift_up(i);
		add_up(i);
		shift_up(i);
	}
}

void move_down()
{
	int i;
	for(i = 0; i < 4; i++)
	{
		shift_down(i);
		add_down(i);
		shift_down(i);
	}
}

void move_left()
{
	int i;
	for(i = 0; i < 4; i++)
	{
		shift_left(i);
		add_left(i);
		shift_left(i);
	}
}

void move_right()
{
	int i;
	for(i = 0; i < 4; i++)
	{
		shift_right(i);
		add_right(i);
		shift_right(i);
	}
}

/*********************** main section *****************************/

int main(int argc, char* argv[])
{
	
	char *st;
	char *file_name = "data";
	int ch, flag = 1;
	char   buffer[10];

	if(argc != 3)
	{
		printf("usage\n 'game_name 1 your_name' to set target 1024\n 'game_name 2 your_name' to set target 2048\n 'game_name 3 your_name' to set target 4096\n");		
		exit(1);
	}
		
	else if(strcmp(argv[1], "1") == 0)
	{
		target = 1024;		
	}
		
	else if(strcmp(argv[1], "2") == 0)
	{
		target = 2048;
		st = "Target set to 2048";
	}
	
	else if(strcmp(argv[1], "3") == 0)
	{
		target = 4098;
		st = "Target set to 4098";
	}
	else
	{
		printf("usage\n 'game_name 1 your_name' to set target 1024\n 'game_name 2 your_name' to set target 2048\n 'game_name 3 your_name' to set target 4096\n");		
		exit(1);
	}
	
	file_name = argv[2];
	
	initscr();		
	curs_set(0);
	keypad(stdscr, TRUE);	
	draw_table(5);	
	
	if(file_exist(file_name))
	{
		read_from_file(file_name);
		update_table();
	}
	else
	{	
		init_table();
		update_table();
		save_table(file_name);
	}
	
	if( target == 1024 )
		st = "Target set to 1024";
	else if( target == 2048 )
		st = "Target set to 2048";
	else if( target == 4096 )
		st = "Target set to 4096";
	print_message(st);
	
	while(flag && has_result == 0)
	{
		ch = getch();
		if( ch == KEY_RIGHT)
		{
			move_right();
			set_random_entry();
			save_table(file_name);
			update_table();
		}
		else if( ch == KEY_LEFT)
		{
			move_left();
			set_random_entry();
			save_table(file_name);
			update_table();
		}
		else if( ch == KEY_UP)
		{
			move_up();
			set_random_entry();
			save_table(file_name);
			update_table();
		}
		else if( ch == KEY_DOWN)
		{
			move_down();
			set_random_entry();
			save_table(file_name);
			update_table();
		}
		
		else if( ch == 10)
		{
			print_message("press enter again to exit");
			ch = getch();
			if( ch == 10)
			{
				flag = 0;
				print_message(st);
				save_table(file_name);
			}
			else
			{
				print_message("                                                ");
			}
		}				
	}	
	
	if( has_result ) {
		
		while(1)
		{
			ch = getch();
			move(init_y-1, init_x);
			addstr(result);
			if( ch == 10)
			{
				init_table();
				update_table();
				save_table(file_name);
				break;
			}	
		}
	}
	endwin();
	return 0;
}

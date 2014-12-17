#include "utils.h"

void reverse( u8 data[], int head, int rear )
{
	int hold;
	int i;
	for(i = head; head < rear; head++, rear-- )
	{
		hold = data[ head ];
		data[ head ] = data[ rear ];
		data[ rear ] = hold;
	}
}

void moveLeftK( u8 data[], int size, int k )
{
	reverse( data, 0, k - 1 );
	reverse( data, k, size - 1 );
	reverse( data, 0, size - 1 );
}

int findPPA(u8 data[],int size)
{
	int res = 0;
	int i;
	for(i=0;i<size-2;i++){
		if(data[i] == 'P' && data[i+1] == 'P' && data[i+2] == 'A') return i;
	}
	return -1;
}


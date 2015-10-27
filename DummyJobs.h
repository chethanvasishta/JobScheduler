#ifndef DUMMYJOBS_H
#define DUMMYJOBS_H

#include <iostream>
#include <math.h>
using namespace std;

void Foo1()
{
	cout << "In foo1" << endl;
	int sum = 0;
	const int size = 1000;
	int **a = new int*[size];
	int **b = new int*[size];
	int **c = new int*[size];
	for(int i = 0 ; i < size ; ++i)
	{
		a[i] = new int[size];
		b[i] = new int[size];
		c[i] = new int[size];
		for(int j = 0 ; j < size ; ++j)
		{
			a[i][j] = i+j;
			b[i][j] = i+j;
			c[i][j] = 0;
		}
	}

	for(int i = 0 ; i < size ; ++i)
		for(int j = 0 ; j < size ; ++j)
			for(int k = 0 ; k < size ; ++k)
				c[i][j] += a[i][k]*b[k][j];

	delete[] a;
	delete[] b;
	delete[] c;
	cout << "Matrix multiply completed!" << endl;
}

#endif // DUMMYJOBS_H
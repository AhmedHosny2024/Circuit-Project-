#include<iostream>
#include <cmath>
#include<string>
#include<fstream>
#include<Eigen/Dense>
#include<Eigen/Sparse>
#include<complex>
#include"vsrc.h"
#include"Capacaitor.h"
#include"Resistance.h"
#include"Inductor.h"
#include"isrc.h"
#include"Branch.h"
#include "Node.h"
#include "Current_C_Current.h"
using namespace std;
using namespace Eigen;
using namespace std::complex_literals;
#include"Component.h"
double W;
Component* searcher(string x);
#define n_max 10
Component* complist[n_max] = { NULL };
int comcount = 0;
/////////////nodes&branches/////////////
int nodecount = 0;
int nonsimple_nodecount = 0;
int simple_nodecount = 0;
int branchcount = 0;
int nodes_rannk[10] = { -1 ,-1,-1,-1,-1,-1,-1,-1,-1,-1};
Node** Nodelist;
Node** nonsimple_Nodelist;
Node** simple_Nodelist;
Branch** fullBranchlist;
Branch** Branchlist;
/////////////nodes&branches/////////////
Isrc * simplification(Vsrc* a, complex<double> b)
{
	complex<double> x = (a->getcomplex()) / b;
	double p = x.real() * x.real() + x.imag() * x.imag();
	double mag = sqrt(p);
	double phase = tanf(x.imag() / x.real());
	Isrc *c =new Isrc(a->get_name(), a->get_node1(), a->get_node2(), phase, mag);
	Branch q(Nodelist[a->get_node1()], Nodelist[a->get_node2()]);
	q.setcurrent(c->getcomplex());
//	c->zerosetters();
	return c;
}
MatrixXcd CalculateVoltNode(MatrixXcd G, MatrixXcd I);
int main()
{
	/*double c, a = 2;
	double b = 2;
	complex <double> c2(a, b);
	c = c2.real() + 1;
	c2.imag(b);
	complex<double> c1;
	c1.imag(1);
	c1.real(0);
	complex<double> c3;
	c3 = c2 / c1;
	Eigen::MatrixXcd s;
	Eigen::Matrix3cd f;
	f << c1, c2, c3, 7.0 + 5i, 8, 9, 10, 11, 12;
	cout << f << endl;*/


	ifstream inputfile;
	ofstream outputfile;
	outputfile.open("circuit.txt");
	inputfile.open("netlist.txt");
	if (!(inputfile.is_open()))
	{
		cout << "file is not opened , try again" << endl;
		return 0;
	}
	string name;

	while (!(inputfile.eof()))
	{
		int n1 = -1, n2 = -1;
		inputfile >> name;
		if (name == "w"|| name =="W")
		{
			double w;
			inputfile >> w;
			W = w;
			//outputfile << name << " " << w << endl;

		}
		else if (name == "vsrc" || name == "Vsrc" || name == "VSrc")
		{
			string Name;
			double phase, magintude;
			inputfile >> Name >> n1 >> n2 >> magintude >> phase;
			Vsrc* v1 = new Vsrc(Name, n1, n2, phase, magintude);
			complist[comcount++] = v1;
			//outputfile << name << " " << Name << " " << n1 << " " << n2 << " " << magintude << " " << phase << endl;

		}
		else if (name == "isrc" || name == "Isrc" || name == "ISrc")
		{

			string Name;
			double phase, magintude;
			inputfile >> Name >> n1 >> n2 >> magintude >> phase;
			Isrc* v1 = new Isrc(Name, n1, n2, phase, magintude);
			complist[comcount++] = v1;
			//outputfile << name << " " << Name << " " << n1 << " " << n2 << " " << magintude << " " << phase << endl;
		}
		else if (name == "vcvs" || name == "Vcvs" || name == "VCvs")
		{
			/*string Name;
			int n3, n4;
			string Name2;
			double val;
			inputfile >> Name >> n1 >> n2 >> n3 >> n4 >> Name2 >> val;*/
			//Vcvs* v1 = new Vcvs(Name, n1, n2, phase, magintude);
			//complist[comcount++] = v1;
			//outputfile << name << " " << Name << " " << n1 << " " << n2 << " " << n3 << " " << n4 <<" "<< Name2<<" "<<val<< endl;
		}
		else if (name == "cccs" || name == "Cccs" || name == "CCcs")
		{
			string Name;
			int n3, n4;
			string Name2;
			double val;
			Current_C_Current* cccs = new Current_C_Current(n1,n2,Name,n3,n4,Name2,val);
			complist[comcount++] = cccs;
		}
		else if (name == "res")
		{
			string Name;
			double val;
			inputfile >> Name >> n1 >> n2 >> val;
			Resistance* v1 = new Resistance(val, Name, n1, n2);
			complist[comcount++] = v1;
			//outputfile << name << " " << Name << " " << n1 << " " << n2 << " " << val << endl;

		}
		else if (name == "cap")
		{
			string Name;
			double val;
			inputfile >> Name >> n1 >> n2 >> val;
			Capacaitor* v1 = new Capacaitor(val, W, Name, n1, n2);
			complist[comcount++] = v1;
			//outputfile << name << " " << Name << " " << n1 << " " << n2 << " " << val << endl;
		}
		else if (name == "ind")
		{
			string Name;
			double val;
			inputfile >> Name >> n1 >> n2 >> val;
			Inductor* v1 = new Inductor(val, W, Name, n1, n2);
			complist[comcount++] = v1;
			//outputfile << name << " " << Name << " " << n1 << " " << n2 << " " << val << endl;
		}
		bool findN1 = false;
		bool findN2 = false;
		for (int i = 0; i < 10; i++)
		{
			if (n1 == nodes_rannk[i])
			{
				findN1 = true;
			}
			if (n2 == nodes_rannk[i])
			{
				findN2 = true;
			}
		}
		if (findN1 == false)
		{
			nodes_rannk[nodecount++] = n1;
		}
		if (findN2 == false)
		{
			nodes_rannk[nodecount++] = n2;
		}

	}

	outputfile.close();
	inputfile.close();
	/////////////////////////////////// Sabry ////////////////////////////////////////////
	Current_C_Current* cccs;
	for (int i = 0; i < comcount; i++)
	{
		cccs = dynamic_cast<Current_C_Current*>(complist[i]);
		if (cccs != NULL)
		{
			break;
		}
	}
	if (cccs != NULL)
	{
		for (int j = 0; j < comcount; j++)
		{
			if (cccs->get_Noded1() == complist[j]->get_node1() && cccs->get_Noded2() == complist[j]->get_node2())
			{
				Resistance* r1 = dynamic_cast <Resistance*>(complist[j]);
				Capacaitor* c1 = dynamic_cast <Capacaitor*>(complist[j]);
				Inductor* in1 = dynamic_cast <Inductor*>(complist[j]);
				if (r1 != NULL)
				{
					cccs->set_Impedance(r1->get_Impedance());
				}
				else if(c1 !=NULL)
				{
					cccs->set_Impedance(c1->get_Impedance());
				}
				else if(in1 !=NULL)
				{
					cccs->set_Impedance(in1->get_Impedance());
				}

			}
		}
	}
	////////////////////madbouly el gamed statr /////////////////////////
	Nodelist = new Node * [nodecount];
	for (int i = 0; i < nodecount; i++)
	{
		Nodelist[i] = new Node(i);
	}

	for (int i = 0; i < comcount; i++)
	{
		int N1 = complist[i]->get_node1();
		int N2 = complist[i]->get_node2();
		int indexnode1, indexnode2;
		for (int i = 0; i < nodecount; i++)
		{
			if (Nodelist[i]->getrank() == N1)
			{
				indexnode1 = i;
			}
			else if (Nodelist[i]->getrank() == N2)
			{
				indexnode2 = i;
			}
		}
		Nodelist[indexnode1]->increment_connections(Nodelist[indexnode2]);
		Nodelist[indexnode2]->increment_connections(Nodelist[indexnode1]);
	}
	for (int i = 0; i < nodecount; i++)
	{
		if (!(Nodelist[i]->ifsimple()))
		{
			nonsimple_nodecount++;
		}
	}
	simple_nodecount = nodecount - nonsimple_nodecount;
	nonsimple_Nodelist = new Node * [nonsimple_nodecount];
	simple_Nodelist = new Node * [simple_nodecount];

	{
		int nonsimple = 0; int simple = 0;
		for (int i = 0; i < nodecount; i++)
		{
			if (Nodelist[i]->ifsimple())
			{
				simple_Nodelist[simple++] = Nodelist[i];
			}
			else
			{
				nonsimple_Nodelist[nonsimple++] = Nodelist[i];
			}
		}
	}

	////////////////////////////// creat branches /////////////////////////////////////
	//fullBranchlist = new Branch * [comcount];
	branchcount = comcount - simple_nodecount;
	Branchlist = new Branch * [branchcount];
	{
		int counter = 0;
		for (int i = 0; i < comcount; i++)
		{
			int N1 = complist[i]->get_node1();
			int N2 = complist[i]->get_node2();
			//fullBranchlist[i] = new Branch(Nodelist[N1], Nodelist[N2]);
			bool find = false;
			for (int i = 0; i < simple_nodecount; i++)
			{
				if (N1 == simple_Nodelist[i]->getrank() || N2 == simple_Nodelist[i]->getrank())
				{
					find = true;
				}
			}
			if (find == false)
			{
				Vsrc* v1 = dynamic_cast <Vsrc*>(complist[i]);
				Isrc* i1 = dynamic_cast <Isrc*>(complist[i]);

				Resistance* r1 = dynamic_cast <Resistance*>(complist[i]);
				Capacaitor* c1 = dynamic_cast <Capacaitor*>(complist[i]);
				Inductor* in1 = dynamic_cast <Inductor*>(complist[i]);

				Branchlist[counter] = new Branch(Nodelist[N1], Nodelist[N2]);
				if (v1 != NULL)
				{
					Branchlist[counter]->setvolt(v1);
				}
				else if (i1 != NULL)
				{
					Branchlist[counter]->setcurentsource(i1);
				}
				else if (r1 != NULL)
				{
					Branchlist[counter]->setz(r1->get_Impedance());
				}
				else if (c1 != NULL)
				{
					Branchlist[counter]->setz(c1->get_Impedance());
				}
				else if (in1 != NULL)
				{
					Branchlist[counter]->setz(in1->get_Impedance());
				}
				counter++;
			}
		}

		for (int i = 0; i < simple_nodecount; i++)
		{
			Node** connection = simple_Nodelist[i]->getNodeconnections();
			int N1 = connection[0]->getrank();
			int N2 = connection[1]->getrank();

			Branchlist[counter] = new Branch(Nodelist[N1], Nodelist[N2]);
			Component* comp1;
			Component* comp2;
			bool find = false;
			for (int i = 0; i < comcount; i++)
			{
				int N3 = complist[i]->get_node1();
				int N4 = complist[i]->get_node2();
				if ((N3 == i && N4 == N1) || (N3 == N1 && N4 == i) || (N3 == i && N4 == N2) || (N3 == N2 && N4 == i))
				{
					find == true;
				}

				if (find == true)
				{
					Vsrc* v1 = dynamic_cast <Vsrc*>(complist[i]);
					Isrc* i1 = dynamic_cast <Isrc*>(complist[i]);

					Resistance* r1 = dynamic_cast <Resistance*>(complist[i]);
					Capacaitor* c1 = dynamic_cast <Capacaitor*>(complist[i]);
					Inductor* in1 = dynamic_cast <Inductor*>(complist[i]);
					if (v1 != NULL)
					{
						Branchlist[counter]->setvolt(v1);
					}
					else if (i1 != NULL)
					{
						Branchlist[counter]->setcurentsource(i1);
					}
					else if (r1 != NULL)
					{
						Branchlist[counter]->setz(r1->get_Impedance());
					}
					else if (c1 != NULL)
					{
						Branchlist[counter]->setz(c1->get_Impedance());
					}
					else if (in1 != NULL)
					{
						Branchlist[counter]->setz(in1->get_Impedance());
					}

				}
			}
			counter++;
		}
	}
	/////////////////////////madbouly el gamed end //////////////////////////
	//int n = 2;
	//Branch** nbranch = new Branch * [n];
	complex<double> zero;
	zero.real(0);
	zero.imag(0);

	Isrc *c;
	for (int i = 0; i < branchcount; i++)
	{
		if (Branchlist[i]->getcurrent().real() == zero.real() && Branchlist[i]->getcurrent().imag() == zero.imag() && Branchlist[i]->getz().real() > zero.real()
			&& Branchlist[i]->getz().imag() > zero.imag() && Branchlist[i]->getvolt()->getcomplex().real() > zero.real() && Branchlist[i]->getvolt()->getcomplex().imag() > zero.imag()) {
			Branch q = *Branchlist[i];
			c =simplification(Branchlist[i]->getvolt(), Branchlist[i]->getz());
			complist[comcount++];
			Branchlist[i]->getvolt()->zerosetters();

		}
	}
	/////////////////////////////////////Ahmed hany
	int num_non_sim_nodes = nonsimple_nodecount;
	int num = num_non_sim_nodes - 1;
	/*Node n1, n2;
	Node* arr[2] = { &n1,&n2 };*/

	int* arr;
	arr = new int[nonsimple_nodecount - 1];
	for (int i = 1; i < nonsimple_nodecount; i++)
	{
		arr[i-1] = nonsimple_Nodelist[i]->getrank();
	}
	Eigen::MatrixXcd m(num, num);
	for (int i = 0; i < num; i++)
	{
		for (int j = 0; j < num; j++)
		{
			m(i, j).real(0);
			m(i, j).imag(0);
		}
	}
	Eigen::MatrixXcd I(num, 1);
	for (int i = 0; i < num; i++)
	{
		I(i, 0) = 0;
	}
	/*m(0, 0).real(3);
	m(0, 0).imag(5);
	m(1, 0).real(3);
	m(1, 0).imag(5);
	m(2, 0).real(3);
	m(2, 0).imag(5);
	m(2, 2).real(3);
	m(2, 2).imag(5);
	cout << m;*/
	for (int j = 0; j < num; j++)
	{
		for (int k = 0; k < num; k++)
		{
			complex <double> Admittance_s(0, 0);
			complex <double> Admittance_d(0, 0);
			for (int i = 0; i < comcount; i++)
			{
				Component* comp = complist[i];
				Resistance* resistance = dynamic_cast<Resistance*>(comp);
				Inductor* inductor = dynamic_cast<Inductor*>(comp);
				Capacaitor* capacaitor = dynamic_cast<Capacaitor*>(comp);
				if (resistance != NULL)
				{
					if ((resistance->get_node1() == arr[k]) || (resistance->get_node2() == arr[k]))
					{
						complex <double> r;
						r = 1.0 / resistance->get_Impedance();
						Admittance_s += r;
					}
					if ((resistance->get_node1() == arr[j]) && (resistance->get_node2() == arr[k]))
					{
						complex <double> r;
						r = 1.0 / resistance->get_Impedance();
						Admittance_d += r;
					}
				}
				else if (inductor != NULL)
				{
					if ((inductor->get_node1() == arr[k]) || (inductor->get_node2() == arr[k]))
					{
						complex <double> in;
						in = inductor->get_Admittance();
						Admittance_s += in;
					}
					if ((inductor->get_node1() == arr[j]) && (inductor->get_node2() == arr[k]))
					{
						complex <double> r;
						r = inductor->get_Admittance();
						Admittance_d += r;
					}
				}
				else if (capacaitor != NULL)
				{
					if ((capacaitor->get_node1() == arr[k]) || (capacaitor->get_node2() == arr[k]))
					{
						complex <double> ca;
						ca = capacaitor->get_Admittance();
						Admittance_s += ca;
					}
					if ((capacaitor->get_node1() == arr[j]) && (capacaitor->get_node2() == arr[k]))
					{
						complex <double> r;
						r = capacaitor->get_Admittance();
						Admittance_d += r;
					}

				}
			}
			if (j == k)
			{
				m(k, k) = Admittance_s;
			}
			else
			{
				m(j, k) = -Admittance_d;
			}
		}
	}
	for (int j = 0; j < num; j++)
	{
		for (int i = 0; i < comcount; i++)
		{
			Component* comp = complist[i];
			Isrc* current = dynamic_cast<Isrc*>(comp);
			if (current != NULL)
			{
				if (current->get_node1() == arr[j])
				{
					complex <double> curr = current->getcomplex();
					I(j, 0) += curr;

				}
				if (current->get_node2() == arr[j])
				{
					complex <double> curr = -current->getcomplex();
					I(j, 0) += curr;
				}
			}
		}
	}
	cout << m << endl;
	cout << I << endl;
	///////////////////////////sabry el gamed///////////////////////
	// Dependent Source CCCs
	int cccs_node1;
	double sign = 1;
	int index;
	if (cccs != NULL)
	{
		for (int i = 1; i < nonsimple_nodecount; i++)
		{
			if (cccs->get_node1() == nonsimple_Nodelist[i]->getrank())
			{
				index = i;
				sign *= 1;
			}
			else if (cccs->get_node2() == nonsimple_Nodelist[i]->getrank())
			{
				index = i;
				sign *= -1;
			}

		}
		for (int i = 1; i < nonsimple_nodecount; i++)
		{
			if(nonsimple_Nodelist[i]->getrank() == cccs->get_Noded1())
			{
				m(index - 1, i - 1) += sign * cccs->get_Admittance() * cccs->get_cofficient();
			}
			else if (nonsimple_Nodelist[i]->getrank() == cccs->get_Noded2())
			{
				m(index - 1, i - 1) -= sign * cccs->get_Admittance() * cccs->get_cofficient();
			}
		}
	}
	MatrixXcd V = CalculateVoltNode(m, I); // function calculate volt of each node
	for (int i = 1; i < nonsimple_nodecount; i++)
	{
		nonsimple_Nodelist[i]->setvoltage(V(i-1, 0));
	}  
	for (int i = 0; i < branchcount; i++)
	{
		Branchlist[i]->Calculatecurrent();
	}
	cout << V;
	// now we have voltage od each node and current in each branch

	return 0;
}

///////////////////////////sabry el gamed///////////////////////
MatrixXcd CalculateVoltNode(MatrixXcd G, MatrixXcd I)
{
	MatrixXcd V;
	V = G.inverse() * I;
	return V;
}
//////////////////////////////Ahmed hany
Component* searcher(string x)
{
	Component* comp = NULL;
	for (int i = 0; i < comcount; i++)
	{
		string name = complist[i]->get_name();
		if ((name.compare(x) == 0))
		{
			comp = complist[i];
		}
	}
	return comp;
}

#include "submit.h"

using namespace std;

void submit_fl(char* s1, const char* s2)
{
	bool fldone = false;
	FILE* file = fopen("fluent.sh", "w");
	fprintf(file, "#!/bin/bash\n");
	fprintf(file, "#SBATCH -J fluent-mcfluent\n");
	fprintf(file, "#SBATCH -p batch\n");
	fprintf(file, "#SBATCH --partition=128G\n");
	fprintf(file, "#SBATCH --nodes=%s\n", s1);
	fprintf(file, "#SBATCH --ntasks-per-node=12\n");
	fprintf(file, "#SBATCH --nodelist=cn[%s]\n", s2);
	fprintf(file, "filedir=\"/vol/home/zhangqingyang/MCF/3x3/FL/\"\n");
	fprintf(file, "MODELTOCOMPUTE=\"1-4.jou\"\n");
	fprintf(file, "INPUTFILE=\"${filedir}${MODELTOCOMPUTE}\"\n");
	fprintf(file, "MYHOSTS=\"${filedir}myhosts\"\n");
	fprintf(file, "OUTPUTFILE=\"${filedir}output${MODELTOCOMPUTE}\"\n");
	fprintf(file, "BATCHLOG=\"${filedir}${MODELTOCOMPUTE}.log\"\n");
	fprintf(file, "FLUENT=/vol/software/ansys/ansys_inc/v172/fluent/bin/fluent\n");
	fprintf(file, "NTASKS=`echo $SLURM_TASKS_PER_NODE | cut -c1-2`\n");
	fprintf(file, "NPROCS=`expr $SLURM_NNODES \\* $NTASKS`\n");
	fprintf(file, "$FLUENT 3d -g -slurm -t$NPROCS -p -cnf=$MYHOSTS -mpi=openmpi -i $INPUTFILE > $OUTPUTFILE\n");
	fclose(file);

	start = clock();
	file = popen("sbatch fluent.sh");
	//file = popen("sleep 5; touch output1-4.jou;cat fluent.sh;sleep 100", "r");
}

void check_finish()
{
	//unique_lock<mutex> lck(mtx);
	FILE* file = fopen("output1-4.jou", "r");
	while (!file)
	{
		file = fopen("output1-4.jou", "r");
		sleep(1);
		//printf("waiting creating jou file\n");
	}
		//printf("jou file created\n");
	bool ready = false;
	//char f1[] = "Exiting";
	//char f2[] = "> /exit";
	//char f3[] = "error";
	while (!ready)
	{
		fgets(buf, 8184, file);
		for (int i = 0; i < 8184; ++i)
		{
			if (buf[i] == 'E')
			{
				if (buf[i + 1] == 'x' && buf[i + 2] == 'i' && buf[i + 3] == 't' && buf[i + 4] == 'i' && buf[i + 5] == 'n' && buf[i + 6] == 'g')
				{
					ready = true;
					fldone = true;
					cv.notify_all();
				}
			}
			else if (buf[i] == '>')
			{
				if (buf[i + 1] == ' ' && buf[i + 2] == '/' && buf[i + 3] == 'e' && buf[i + 4] == 'x' && buf[i + 5] == 'i' && buf[i + 6] == 't')
				{
					ready = true;
					fldone = true;
					cv.notify_all();
				}

			}
			else if (buf[i] == 'e')
			{
				if (buf[i + 1] == 'r' && buf[i + 2] == 'r' && buf[i + 3] == 'o' && buf[i + 4] == 'r')
				{
					ready = true;
					fldone = true;
					cv.notify_all();
				}

			}
		}
		sleep(1);
	}
	fclose(file);	
}

double allend()
{
	//unique_lock<mutex> lck(mtx);
	while (!fldone);
		//cv.wait(lck);
	finish = clock();
	double timeoffl = (finish - start) / 60000.0;
	return timeoffl;
}
//=============================================================================================
void submit_mc(char* s1, const char* s2)
{
	FILE* file = fopen("openmc.sh", "w");
	fprintf(file, "#!/bin/bash\n");
	fprintf(file, "#SBATCH -J openmc-mcfluent\n");
	fprintf(file, "#SBATCH -p batch\n");
	fprintf(file, "#SBATCH --partition=128G\n");
	fprintf(file, "#SBATCH --nodes=%s\n", s1);
	fprintf(file, "#SBATCH --ntasks-per-node=12\n");
	fprintf(file, "#SBATCH --nodelist=cn[%s]\n", s2);
	fprintf(file, "filedir=\"/vol/home/zhangqingyang/MCF/3x3/MC/\"\n");
	fprintf(file, "OUTPUTFILE=\"${filedir}mcresult\"\n");
	fprintf(file, "BATCHLOG=\"${filedir}mcrunning.log\"\n");
	fprintf(file, "OPENMC=/vol/zhangqingyang/openmc/openmc-install/bin/openmc\n");
	fprintf(file, "NTASKS=`echo $SLURM_TASKS_PER_NODE | cut -c1-2`\n");
	fprintf(file, "NPROCS=`expr $SLURM_NNODES \\* $NTASKS`\n");
	fprintf(file, "mpiexec -n$NPROCS $OPENMC &> $OUTPUTFILE\n");
	fclose(file);

	start2 = clock();
	file = popen("sbatch openmc.sh");
}

void check_finish2()
{
	//unique_lock<mutex> lck(mtx);
	FILE* file = fopen("mcresult", "r");
	while (!file)
	{
		file = fopen("mcresult", "r");
		sleep(1);
		//printf("waiting creating mcresult file\n");
	}
		//printf("mcresult file created\n");
	bool ready = false;
	//char f1[] = "Exiting";
	//char f2[] = "> /exit";
	//char f3[] = "error";
	while (!ready)
	{
		fgets(buf, 8184, file);
		for (int i = 0; i < 8184; ++i)
		{
			if (buf[i] == 'L')
			{
				if (buf[i + 1] == 'e' && buf[i + 2] == 'a' && buf[i + 3] == 'k' && buf[i + 4] == 'a' && buf[i + 5] == 'g' && buf[i + 6] == 'e')
				{
					ready = true;
					mcdone = true;
					cv.notify_all();
				}
			}
			else if (buf[i] == 'e')
			{
				if (buf[i + 1] == 'r' && buf[i + 2] == 'r' && buf[i + 3] == 'o' && buf[i + 4] == 'r')
				{
					ready = true;
					mcdone = true;
					cv.notify_all();
				}

			}
		}
		sleep(1);
	}
	fclose(file);	
}

double allend2()
{
	//unique_lock<mutex> lck(mtx);
	while (!mcdone);
		//cv.wait(lck);
	finish2 = clock();
	double timeofmc = (finish2 - start2) / 60000.0;
	return timeofmc;
}
//================================================================================================
double run_fl(char* s1, char* s2)
{
	double time_fl;
	submit_fl(s1, s2);
	check_finish();
	time_fl = allend();

	return time_fl;
}

double run_mc(char* s1, char* s2)
{
	double time_mc;
	submit_mc(s1, s2);
	check_finish2();
	time_mc = allend2();

	return time_mc;
}
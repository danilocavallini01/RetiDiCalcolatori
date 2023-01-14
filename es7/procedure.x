const FILELENGTH = 100;

struct FileScanRes{
	int characters;
    int words;
    int lines;
    int errno;
};


struct DirInput {
    int threshold;
    string dir<100>;
};


program PROCEDUREPROG {
	version PROCEDUREVERS {
		FileScanRes FILESCAN(string<100>) = 1;
		int DIRSCAN(DirInput) = 2;
	} = 1;
} = 0x20000013;
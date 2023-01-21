const FILE_LENGTH = 50;
const MAX_RESPONSE_FILES = 6;

struct Request {
    char nomedir[FILE_LENGTH];
    char chr;
    int noccorrenze;
};

struct RFile {
    char nomefile[FILE_LENGTH];
};

struct Response {
    RFile files[MAX_RESPONSE_FILES];
    int size;
    int error;
};

program OPERATION {
    version OPERATIONVERS {
        int elimina_occorrenze(string nomefile<FILE_LENGTH>) = 1;
        Response lista_file_carattere(Request request) = 2;
    } = 1;
} = 0x20000013;
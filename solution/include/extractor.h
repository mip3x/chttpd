#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#define REFERENCED_FILES 10
#define BUFFER_SIZE 512

void extract_links(const char* file_path, const char* web_root);
void process_routes();

#endif

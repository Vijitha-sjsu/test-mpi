#ifndef DATALOADER_H
#define DATALOADER_H

#include <string>
#include <vector>

class DataLoader {
public:
    explicit DataLoader(const std::string& filename);
    std::vector<std::string> loadData();

private:
    std::string filename;
};

#endif // DATALOADER_H


#include "../inc/FileParser.hpp"

/*::: CONSTRUCTORS :::*/

FileParser::FileParser(std::string const &filePath) : _filePath(filePath) {

	this->_nServers = 0;

	if (PRINT)
		std::cout << GREEN << "Constructor: FileParser created " << END_COLOR << std::endl;

	return ;
}

/*::: DESTRUCTORS :::*/

FileParser::~FileParser() {

	if (PRINT)
		std::cout << RED << "Destructor: FileParser destroyed " << END_COLOR << std::endl;

	return ;
}

std::ostream &operator<<(std::ostream &out, FileParser const &other) {

	out << "FilePath : "
			<< other.getFilePath()
			<< "\n\nRAWFile : "
			<< returnStringVector(other.getRawFile())
			<< "\n\nRAWServer : "
			<< returnStringVector(other.getRawServer())
			<< "\n\nnServers : "
			<< other.getNServers()
			<< "\n";

			// allServers missing : vector of ServerInfo

	return out;
}

/*::: ACCESSORS :::*/

std::string const &FileParser::getFilePath() const {

	return this->_filePath;
}

std::vector<std::string> FileParser::getRawFile() const {

	return this->_rawFile;
}

std::vector<std::string> FileParser::getRawServer() const {

	return this->_rawServer;
}

std::vector<ServerInfo> FileParser::getAllServers() const {

	return this->_allServers;
}

int FileParser::getNServers() const {

	return this->_nServers;
}

/*::: MEMBER FUNCTIONS :::*/

void FileParser::cleanFile() {

	std::string allContent;
	
	allContent = checkFileValid();
	eraseComments(allContent);
	this->_rawFile = cSplitLine(allContent, " \f\t\n\r\v");

	if (this->_rawFile.empty())
		throw FileParserError("File is empty");

}

std::string FileParser::checkFileValid() {

	struct stat fileStat;
	std::ifstream fileStream;
	std::stringstream allContent;
	std::string res;

	if (_filePath.empty() || !_filePath.length())
		throw FileParserError("Invalid path");
	if (stat(_filePath.c_str(), &fileStat) == -1)
		throw FileParserError("Permission denied or Bad address");
	if (!(fileStat.st_mode & S_IFREG))
		throw FileParserError("Invalid file");
	if (access(_filePath.c_str(), R_OK) == -1)
		throw FileParserError("Not reading rights");
	if (_filePath.find(".conf", _filePath.size() - 5) == std::string::npos) // .conf accepted
		throw FileParserError("Wrong configuration file format");
	
	fileStream.open(_filePath.c_str());
	if (fileStream.fail())
		throw FileParserError("Failed to open file");
	
	allContent << fileStream.rdbuf();
	fileStream.close();

	return allContent.str();
}

void FileParser::splitServers() {

	size_t it;
	size_t end = 0;
	int checkServ = 0;

	if (_rawFile[0].compare("server") != 0)
			throw FileParserError("Missing server in configuration file");

	for (it = 0; it != _rawFile.size(); it++)
	{
		if (!_rawFile[it].compare("server"))
			checkServ++;
	}

	for (it = 0; it != _rawFile.size(); it++)
	{
		if (!_rawFile[it].compare("server"))
		{
			it = end;
			if (it + 1 == _rawFile.size())
				throw FileParserError("Wrong Server configuration");
			else if (!_rawFile[it + 1].compare("{"))
			{
				this->_nServers++;
				end = serverEnd(it);
			}
			else
				throw FileParserError("Wrong keyword configuration");
		}
	}
	if (_nServers != checkServ)
		throw FileParserError("Wrong number of servers");
	return;
}

size_t FileParser::serverEnd(size_t pos) {
	
	std::string res;
	bool closed = false;
	bool location = false;
	size_t it;

	if (pos == _rawFile.size())
		throw FileParserError("The end");

	for (it = pos + 2; it != _rawFile.size(); it++)
	{
		if (!_rawFile[it].compare("server")) // we check for next keyword server
		{
			this->_rawServer.push_back(res);
			return (it);
		}
		else if (!_rawFile[it].compare("{") && !location) // we open location
			location = true;
		else if (!_rawFile[it].compare("}") && location) // we close location
			location = false;
		else if (!_rawFile[it].compare("}") && !closed) // we close all
			closed = true;
		else if (!_rawFile[it].compare("server") && _rawFile[it + 1].compare("{")) // check double server name
			throw FileParserError("Wrong keyword configuration");
		else if ((!_rawFile[it].compare("{") && location) || (!_rawFile[it].compare("}") && closed))
			throw FileParserError("Wrong brackets configuration");
		else
			res += _rawFile[it] + " ";
	}
	if (res.empty() || location || !closed)
		throw FileParserError("Wrong Server configuration");
	this->_rawServer.push_back(res);
	return it;
}


ServerInfo	FileParser::stockInfos(std::vector<std::string> serverTab) {
	
	ServerInfo newServer;
	bool location = false;
	bool error_page = false;

	for (size_t it = 0; it < serverTab.size(); it++)
	{
		if (!serverTab[it].compare("server_name") && !location)
		{
			it++;
			if(it < serverTab.size() && *(serverTab[it].rbegin()) == ';')
			{
				if (newServer.getServerName() != "")
					throw FileParserError("Server must have only one Name directive");
				serverTab[it].erase(serverTab[it].end() - 1);
				newServer.setServerName(serverTab[it]);
				std::cout << serverTab[it] << std::endl;
			}
			else
				throw FileParserError("server_name configuration error");
		}
		else if (!serverTab[it].compare("host") && !location)
		{
			it++;
			if(it < serverTab.size() && *(serverTab[it].rbegin()) == ';')
			{
				if (newServer.getHost())
					throw FileParserError("Server must have only one Host directive");
				serverTab[it].erase(serverTab[it].end() - 1);
				newServer.setHost(serverTab[it]);
				std::cout << serverTab[it] << std::endl;
			}
			else
				throw FileParserError("host configuration error");
		}
		else if (!serverTab[it].compare("listen") && !location)
		{
			it++;
			if(it < serverTab.size() && *(serverTab[it].rbegin()) == ';')
			{
				serverTab[it].erase(serverTab[it].end() - 1);
				newServer.setPort(serverTab[it]);
				std::cout << serverTab[it] << std::endl;
			}
			else
				throw FileParserError("listen configuration error");
		}
		else if (!serverTab[it].compare("root") && !location)
		{
			it++;
			if(it < serverTab.size() && *(serverTab[it].rbegin()) == ';')
			{
				serverTab[it].erase(serverTab[it].end() - 1);
				newServer.setRoot(serverTab[it]);
				std::cout << serverTab[it] << std::endl;
			}
			else
				throw FileParserError("root configuration error");
		}
		else if (!serverTab[it].compare("index") && !location)
		{
			it++;
			if(it < serverTab.size() && *(serverTab[it].rbegin()) == ';')
			{
				serverTab[it].erase(serverTab[it].end() - 1);
				newServer.setIndex(serverTab[it]);
				std::cout << serverTab[it] << std::endl;
			}
			else
				throw FileParserError("index configuration error");
		}
		else if (!serverTab[it].compare("client_max_body_size") && !location)
		{
			it++;
			if(it < serverTab.size() && *(serverTab[it].rbegin()) == ';')
			{
				serverTab[it].erase(serverTab[it].end() - 1);
				newServer.setMaxClientBody(serverTab[it]);
				std::cout << serverTab[it] << std::endl;
			}
			else
				throw FileParserError("client_max_size configuration error");
		}
		else if (!serverTab[it].compare("timeout") && !location)
		{
			it++;
			if(it < serverTab.size() && *(serverTab[it].rbegin()) == ';')
			{
				serverTab[it].erase(serverTab[it].end() - 1);
				newServer.setTimeout(serverTab[it]);
				std::cout << serverTab[it] << std::endl;
			}
			else
				throw FileParserError("timeout configuration error");
		}
		else if ((!serverTab[it].compare("error_page") && !location) || error_page)
		{
			error_page = true;
			it++;
			if(it < serverTab.size()) // check next
				std::cout << "we set error_pages here!" << std::endl;
			// else
			// 	throw FileParserError("error_page configuration error");
		}
		else if (!serverTab[it].compare("location") || location)
		{
			location = true;
			it++;
			if(it < serverTab.size() && *(serverTab[it].begin()) == '/') //check path
				std::cout << "we set locations here!" << std::endl;	//check if root is set, path, stock everything else
			// else
			// 	throw FileParserError("location configuration error");
		}
		else
			throw FileParserError("Unexpected directive in configuration file");

	}
	std::cout << newServer << std::endl;

	return newServer;
	
}



void	FileParser::stockServerInfo() {

	std::vector<std::string> serverSplit;
	int it;

	for (it = 0; it < _nServers; it++)
	{
		serverSplit = cSplitLine(_rawServer[it], " ");
		this->_allServers.push_back(stockInfos(serverSplit));
	}
	//printServersInfo(_allServers);
}

/*::: EXCEPTIONS :::*/

FileParser::FileParserError::FileParserError(std::string errorMsg) throw() : _errorMsg("Webserv Error : " + errorMsg) {}

FileParser::FileParserError::~FileParserError() throw() {}

const char *FileParser::FileParserError::what() const throw() {

	return (_errorMsg.c_str());
}

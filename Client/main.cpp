#include <string.h>
#include <cstdlib>
#include <cstdint>

#ifdef _DEBUG
#define DLL_PATH "Plugins/Debug"
#else
#define DLL_PATH "Plugins/Release"
#endif

int main(int argc, char** argv)
{
	if (argc != 3)
		return EXIT_FAILURE;

	System::Collections::Generic::Dictionary<System::String^, array<unsigned char>^>^ data = gcnew System::Collections::Generic::Dictionary<System::String^, array<unsigned char>^>();
	for each (System::String ^ filePath in System::IO::Directory::GetFiles(DLL_PATH))
		if (filePath->EndsWith(".dll") && !filePath->EndsWith("Interface.dll"))
		{
			data->Add(System::IO::Path::GetFileName(filePath), System::IO::File::ReadAllBytes(filePath));
		}

	System::Text::ASCIIEncoding^ aSCIIEncoding = gcnew System::Text::ASCIIEncoding();
	System::String^ fileData = "some data sent to the server";
	data->Add("file_data", aSCIIEncoding->GetBytes(fileData));

	System::IO::MemoryStream^ memoryStream = gcnew System::IO::MemoryStream();

	System::Runtime::Serialization::Formatters::Binary::BinaryFormatter^ binaryFormatter = gcnew System::Runtime::Serialization::Formatters::Binary::BinaryFormatter();
	binaryFormatter->Serialize(memoryStream, data);

	System::Net::IPAddress^ ipAddress = System::Net::IPAddress::Parse(gcnew System::String(argv[1]));
	const uint16_t port = static_cast<const uint16_t>(atoll(argv[2]));

	System::Net::IPEndPoint^ serverAddress = gcnew System::Net::IPEndPoint(ipAddress, port);

	System::Net::Sockets::Socket^ server = gcnew System::Net::Sockets::Socket(
		System::Net::Sockets::SocketType::Stream,
		System::Net::Sockets::ProtocolType::Tcp
	);
	server->Connect(serverAddress);

	array<unsigned char>^ dataToSend = memoryStream->ToArray();
	server->Send(System::BitConverter::GetBytes( dataToSend->Length));
	server->Send(dataToSend);
	server->Close();

	return EXIT_SUCCESS;
}
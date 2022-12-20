#include <cstdint>
#include <stdlib.h>

public ref class ServerContext : public AppDomainTest::IContext, public System::MarshalByRefObject
{
public:
	virtual property System::String^ Owner;
	virtual property System::String^ Data;
};

int main(int argc, char** argv)
{
	if (argc != 2)
		return EXIT_FAILURE;

	const uint16_t port = static_cast<const uint16_t>(atoll(argv[1]));

	System::Net::Sockets::Socket^ listener = gcnew System::Net::Sockets::Socket(
		System::Net::Sockets::SocketType::Stream,
		System::Net::Sockets::ProtocolType::Tcp
	);

	System::Net::IPEndPoint^ serverAddress = gcnew System::Net::IPEndPoint(System::Net::IPAddress::Any, port);
	listener->Bind(serverAddress);
	listener->Listen(99);

	array<unsigned char>^ receivedData = gcnew array<unsigned char>(4096 * 1024);
	System::Runtime::Serialization::Formatters::Binary::BinaryFormatter^ binaryFormatter = gcnew System::Runtime::Serialization::Formatters::Binary::BinaryFormatter();
	System::Text::ASCIIEncoding^ aSCIIEncoding = gcnew System::Text::ASCIIEncoding();
	
	while (true)
	{
		System::Net::Sockets::Socket^ connection = listener->Accept();

		connection->Receive(receivedData, 0, sizeof(int), System::Net::Sockets::SocketFlags::None);
		int dataLength = System::BitConverter::ToInt32(receivedData, 0);
		connection->Receive(receivedData, 0, dataLength, System::Net::Sockets::SocketFlags::None);

		System::IO::MemoryStream^ memoryStream = gcnew System::IO::MemoryStream(receivedData);
		System::Collections::Generic::Dictionary<System::String^, array<unsigned char>^>^ data =
			safe_cast<System::Collections::Generic::Dictionary<System::String^, array<unsigned char>^>^>(binaryFormatter->Deserialize(memoryStream));

		System::Type^ pluginInterfaceType = AppDomainTest::IPlugin::typeid;

		ServerContext^ context;

		System::Collections::Generic::Dictionary<System::String^, array<unsigned char>^>^ untrustedPlugins =
			gcnew System::Collections::Generic::Dictionary<System::String^, array<unsigned char>^>();


		for each (auto byteStream in data)
		{
			if (byteStream.Key == "file_data")
			{
				System::String^ fileData = aSCIIEncoding->GetString(byteStream.Value);
				context = gcnew ServerContext();
				context->Owner = connection->RemoteEndPoint->ToString();
				context->Data = fileData;
			}
			else
				untrustedPlugins[byteStream.Key] = byteStream.Value;
		}

		AppDomainTest::Sandbox^ sandbox = AppDomainTest::Sandbox::Create(untrustedPlugins);

		System::Console::Write("Data state : "); System::Console::WriteLine(context->Data);
		sandbox->ExecuteUntrustedCode(context);
		System::Console::Write("Data state : "); System::Console::WriteLine(context->Data);
	}

	return EXIT_SUCCESS;
}
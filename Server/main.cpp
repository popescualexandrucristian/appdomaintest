#include <cstdint>
#include <stdlib.h>

ref class ServerContext sealed : public AppDomainTest::Context
{
	System::String^ owner;
	System::String^ data;

public:
	ServerContext(System::String^ owner) :
		owner(owner) {}

	virtual property System::String^ Owner { 
		System::String^ get();
	}

	virtual void SetData(System::String^ newData)
	{
		data = newData;
	}
	virtual System::String^ GetData()
	{
		return data;
	}
};

System::String^ ServerContext::Owner::get()
{
	return owner;
}

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

		System::Collections::Generic::Dictionary<System::String^, System::Reflection::Assembly^>^ untrustedPlugins =
			gcnew System::Collections::Generic::Dictionary<System::String^, System::Reflection::Assembly^>();
		for each (auto byteStream in data)
		{
			if (byteStream.Key == "file_data")
			{
				System::String^ fileData = aSCIIEncoding->GetString(byteStream.Value);
				context = gcnew ServerContext(connection->RemoteEndPoint->ToString());
				context->SetData(fileData);
			}
			else
				untrustedPlugins[byteStream.Key] = System::Reflection::Assembly::Load(byteStream.Value);
		}
		for each (auto plugin in untrustedPlugins)
		{
			System::Console::Write("Started execution of "); System::Console::Write(plugin.Key); System::Console::Write(" on "); System::Console::WriteLine(context->Owner);
			array<System::Type^>^ types = plugin.Value->GetTypes();
			for each (auto type in types)
			{
				System::Console::Write("Data state : "); System::Console::WriteLine(context->GetData());
				bool isAssignableFrom = pluginInterfaceType->IsAssignableFrom(type);
				if (isAssignableFrom)
				{
					AppDomainTest::IPlugin^ plugin = safe_cast<AppDomainTest::IPlugin^>(System::Activator::CreateInstance(type));
					plugin->Execute(context);
				}
			}
			System::Console::Write("Data state : "); System::Console::WriteLine(context->GetData());
			System::Console::Write("Ended execution of "); System::Console::Write(plugin.Key); System::Console::Write(" on "); System::Console::WriteLine(context->Owner);
		}
	}


	return EXIT_SUCCESS;
}
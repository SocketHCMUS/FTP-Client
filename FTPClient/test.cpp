std::cout<<"Sending File : "<<getFileName(args)<<" ...."<<std::endl;
		string data;

		double c_length=length;
		
		// send all data to server.
		while (length>0){
			int read_sz = MAXRECV<length ? MAXRECV : length;
			char buf[MAXRECV+1];
			in.read(buf,read_sz);
			data.assign(buf,read_sz);
			*data_socket<<data;
			length -= read_sz;
		}
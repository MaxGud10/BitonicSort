__kernel void bitonic_sort(
    __global int *data_buf, int data_size) {
        
    for (size_t block_size = 2; block_size <= data_size; block_size *= 2)
	{
        for (size_t lhs_id = 0; lhs_id < data_size; lhs_id += block_size)
		{
            bool is_ascending = ((lhs_id / block_size) % 2 == 0);

			for (size_t k = block_size / 2; k > 0; k /= 2)
				for (size_t i = lhs_id; i + k < lhs_id + block_size; ++i)
					if ((data_buf[i] > data_buf[i + k]) == is_ascending)
					{
                        int tmp = data_buf[i];
                        data_buf[i] = data_buf[i + k];
                        data_buf[i + k] = tmp;
					}
        }
    }
}  
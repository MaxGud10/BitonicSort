__kernel void global_bmerge(__global int *vec, uint cur_stage, uint passed_stage, uint incr_order)
{
    uint glob_id  = get_global_id(0);
    uint distance = 1 << (cur_stage - passed_stage);

    uint left_id  = (glob_id % distance) + (glob_id / distance) * 2 * distance;
    uint right_id = left_id + distance;

    int greater = (vec[left_id] > vec[right_id]) ? vec[left_id]  : vec[right_id];
    int less    = (vec[left_id] > vec[right_id]) ? vec[right_id] : vec[left_id];

    if (( glob_id / (1 << cur_stage)) % 2 == 1 )
        incr_order = 1 - incr_order;

    vec[left_id]  = incr_order ? less    : greater;
    vec[right_id] = incr_order ? greater : less;
}

__kernel void local_bsort(__global int *vec, uint cur_stage, __local int *local_data, uint incr_order)
{
    uint local_id       = get_local_id(0);
	uint cur_incr_order = incr_order;
    uint group_size     = get_local_size(0);
    uint gr_offset      = get_group_id(0) * group_size;

    local_data[local_id]              = vec[gr_offset * 2 + local_id];
    local_data[local_id + group_size] = vec[gr_offset * 2 + local_id + group_size];

    barrier(CLK_LOCAL_MEM_FENCE);

    for(uint pair = 0; pair < cur_stage; ++pair)
    {
        for(uint pair_passed = 0; pair_passed < pair + 1; ++pair_passed)
        {
            uint cmp_dist = 1 << (pair - pair_passed);

            uint left_id  = (local_id % cmp_dist) + (local_id / cmp_dist) * 2 * cmp_dist;
            uint right_id = left_id + cmp_dist;

            int left_elem  = local_data[left_id];
            int right_elem = local_data[right_id];

            if((local_id / (1 << pair)) % 2 == 1)
                cur_incr_order = 1 - cur_incr_order;

            int greater = (left_elem > right_elem) ? left_elem  : right_elem;
            int less    = (left_elem > right_elem) ? right_elem : left_elem;

            local_data[left_id]  = cur_incr_order ? less    : greater;
            local_data[right_id] = cur_incr_order ? greater : less;

			cur_incr_order = incr_order;

            barrier(CLK_LOCAL_MEM_FENCE);
        }
    }

    vec[gr_offset * 2 + local_id]              = local_data[local_id];
    vec[gr_offset * 2 + local_id + group_size] = local_data[local_id + group_size];
}

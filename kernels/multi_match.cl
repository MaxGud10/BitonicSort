__kernel void multi_match_count(__global const uchar* text,
                                uint                 text_size,
                                __global const int*  transitions,
                                __global const uint* out_offsets,
                                __global const uint* out_pattern_ids,
                                volatile __global uint* counts)
{
    const uint start_pos = get_global_id(0);

    if (start_pos >= text_size)
        return;

    int state = 0;

    for (uint pos = start_pos; pos < text_size; ++pos)
    {
        const uint ch = (uint)text[pos];

        const int next_state =
            transitions[((uint)state << 8) + ch]; // state * 256 + ch

        if (next_state < 0)
            break;

        state = next_state;

        const uint out_begin = out_offsets[(uint)state];
        const uint out_end   = out_offsets[(uint)state + 1u];

        for (uint i = out_begin; i < out_end; ++i)
        {
            const uint pattern_id = out_pattern_ids[i];
            atomic_inc(&counts[pattern_id]);
        }
    }
}
#include <Processors/QueryPlan/FillingStep.h>
#include <Processors/Transforms/FillingTransform.h>
#include <Processors/QueryPipeline.h>

namespace DB
{

namespace ErrorCodes
{
    extern const int LOGICAL_ERROR;
}

static ITransformingStep::DataStreamTraits getTraits()
{
    return ITransformingStep::DataStreamTraits
    {
            .preserves_distinct_columns = false, /// TODO: it seem to actually be true. Check it later.
            .returns_single_stream = true,
            .preserves_number_of_streams = true,
    };
}

FillingStep::FillingStep(const DataStream & input_stream_, SortDescription sort_description_)
    : ITransformingStep(input_stream_, input_stream_.header, getTraits())
    , sort_description(std::move(sort_description_))
{
    if (!input_stream_.has_single_port)
        throw Exception("FillingStep expects single input", ErrorCodes::LOGICAL_ERROR);
}

void FillingStep::transformPipeline(QueryPipeline & pipeline)
{
    pipeline.addSimpleTransform([&](const Block & header)
    {
        return std::make_shared<FillingTransform>(header, sort_description);
    });
}

}

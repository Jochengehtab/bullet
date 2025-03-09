/*
This is about as simple as you can get with a network, the arch is
    (768 -> HIDDEN_SIZE)x2 -> 1
and the training schedule is pretty sensible.
There's potentially a lot of elo available by adjusting the wdl
and lr schedulers, depending on your dataset.
*/
use bullet_lib::default::loader::MontyBinpackLoader;
use bullet_lib::{
    nn::{optimiser, Activation},
    trainer::{
        default::{
            inputs, outputs, Loss, TrainerBuilder,
        },
        schedule::{lr, wdl, TrainingSchedule, TrainingSteps},
        settings::LocalSettings,
    },
};

use montyformat::chess::{Move, Position};

const HIDDEN_SIZE: usize = 1536;
const SCALE: i32 = 400;
const QA: i16 = 255;
const QB: i16 = 64;

fn main() {
    let mut trainer = TrainerBuilder::default()
        .quantisations(&[QA, QB])
        .optimiser(optimiser::AdamW)
        .loss_fn(Loss::SigmoidMSE)
        .input(inputs::Chess768)
        .output_buckets(outputs::MaterialCount::<8>::default())
        .feature_transformer(HIDDEN_SIZE)
        .activate(Activation::CReLU)
        .add_layer(1)
        .build();

    trainer.load_from_checkpoint("C:\\NNUE-Trainer\\checkpoints\\simple-110\\");

    let schedule = TrainingSchedule {
        net_id: "simple".to_string(),
        eval_scale: SCALE as f32,
        steps: TrainingSteps {
            batch_size: 16_384,
            batches_per_superbatch: 6104,
            start_superbatch: 111,
            end_superbatch: 710,
        },
        wdl_scheduler: wdl::ConstantWDL { value: 0.75 },
        lr_scheduler: lr::StepLR { start: 0.001, gamma: 0.1, step: 180 },
        save_rate: 10,
    };

    trainer.set_optimiser_params(optimiser::AdamWParams::default());

    let settings = LocalSettings { threads: 4, test_set: None, output_directory: "checkpoints", batch_queue_size: 64 };

    let filter = |pos: &Position, mv: Move, score: i16, _result: f32| -> bool { pos.fullm() >= 8
        && score.unsigned_abs() < 10000
        && !mv.is_capture()
        && !pos.in_check() };
    //let data_loader = loader::DirectSequentialDataLoader::new(&["C:\\NNUE-Trainer\\shuffled.bin"]);

    let data_loader = MontyBinpackLoader::new("hugemonty.binpack", 2048, 16, filter);
    trainer.run(&schedule, &settings, &data_loader);
}


let random_line () => (
  1.0 +. (float_of_int (Random.int 10)),
  Presenter.hsl 0 0 (Random.int 80)
);

let random_options canvas_size => {
  let options = Show.Options.{
    canvas_size,
    min_margin: 50.0,
    size_hint: 6 + Random.int 10,
    draw_edges: Random.bool () ? Some (random_line ()) : None,
    draw_shapes: Random.bool () ? Some {
      let hue = (Random.float 360.0);
      let saturation = (70.0 +. Random.float 30.0);
      (fun current max => Presenter.hslf hue saturation (current *. 100.0))
    } : None,
    draw_walls: Random.bool () ? Some (random_line ()) : None,
  };
  (options.draw_edges == None && options.draw_shapes == None && options.draw_walls == None)
    ? {...options, draw_walls: Some (random_line ())}
    : options;
};

let run ctx canvas_size => {
  let seed = [%bs.raw "parseInt(Math.random() * Number.MAX_SAFE_INTEGER)"];
  /*let seed = [%bs.raw "6572995174978857"];*/
  /*let seed = [%bs.raw "5246800480144799"];*/
  Random.init seed;
  Js.log ("Seed", seed);

  let options = random_options canvas_size;

  let choose arr => {
    Array.get arr (Random.int (Array.length arr))
  };

  let random_board () => choose [|
    (module Circle: SimpleBoard.T),
    (module HexBox: SimpleBoard.T),
    (module HexHex: SimpleBoard.T),
    (module TriangleBoard),
    (module NewHexTriangle),
    (module NewRect),
  |];

  let random_alg () => choose [|
    (module (NewDepth.F (NewDepth.RandomConfig ())) : Generator.T),
    (module Random2),
    /*(module NewBFS)*/
  |];

  let module Board = (val random_board());

  let module Gen = (val random_alg ());
  /*let module Board = HexBox;*/

  let module Paint' = Paint.F Board Gen;
  let module Show' = Show.F Board Gen;

  let state = Show'.init_state options;

  Show'.loop options ctx state;
  Canvas.Ctx.setFont ctx "32px monospace";
  Canvas.Ctx.setFillStyle ctx "black";
  Canvas.Ctx.fillText ctx [%bs.raw "seed.toString(16)"] 5.0 30.0;

  seed;
};